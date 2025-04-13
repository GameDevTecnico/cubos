#include <glm/gtc/random.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/collision_layers.hpp>
#include <cubos/engine/collisions/collision_mask.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/gizmos/target.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/physics/constraints/distance_constraint.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective.hpp>
#include <cubos/engine/render/defaults/plugin.hpp>
#include <cubos/engine/render/defaults/target.hpp>
#include <cubos/engine/render/tone_mapping/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::geom::Box;
using cubos::core::io::Key;
using cubos::core::io::Modifiers;

using namespace cubos::engine;

static const Asset<InputBindings> BindingsAsset = AnyAsset("53d85d6f-42a8-4eb8-aeae-1754a4cb20df");

struct Options
{
    CUBOS_ANONYMOUS_REFLECT(Options);

    bool fixedDistance = true;
};

struct State
{
    CUBOS_ANONYMOUS_REFLECT(State);

    Entity a;
    Entity b;
};

int main(int argc, char** argv)
{
    auto cubos = Cubos(argc, argv);

    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(fixedStepPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(inputPlugin);
    cubos.plugin(renderDefaultsPlugin);
    cubos.plugin(gizmosPlugin);
    cubos.plugin(collisionsPlugin);
    cubos.plugin(physicsPlugin);
    cubos.plugin(physicsSolverPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.tag(gizmosDrawTag).after(toneMappingTag);

    cubos.resource<State>();
    cubos.resource<Options>();

    cubos.startupSystem("configure Assets").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("setup input").tagged(assetsTag).call([](const Assets& assets, Input& input) {
        auto bindings = assets.read<InputBindings>(BindingsAsset);
        input.bind(*bindings);
    });

    cubos.startupSystem("setup camera").call([](Commands commands) {
        auto targetEnt = commands.create().add(RenderTargetDefaults{}).add(GizmosTarget{}).entity();

        commands.create()
            .relatedTo(targetEnt, DrawsTo{})
            .add(Camera{.zNear = 0.1F, .zFar = 100.0F})
            .add(PerspectiveCamera{.fovY = 60.0F})
            .add(LocalToWorld{})
            .add(Position{{-4.0F, 1.5F, 0.0F}})
            .add(Rotation{Rotation::lookingAt({3.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
            .add(FreeCameraController{.phi = -15.0F, .theta = 90.0F});
    });

    cubos.startupSystem("create colliders").call([](State& state, Commands commands) {
        state.a = commands.create()
                      .add(Collider{})
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, -2.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {2.0F, 0.0F, 1.0F}})
                      .entity();

        state.b = commands.create()
                      .add(Collider{})
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 2.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {-2.0F, 0.0F, 1.0F}})
                      .entity();

        commands.relate(
            state.a, state.b,
            DistanceConstraint{.length = 5.0F, .localAnchor1 = {0.0F, 0.0F, 0.0F}, .localAnchor2 = {0.0F, 0.0F, 0.0F}});
    });

    cubos.system("activate free camera").call([](const Input& input, Query<FreeCameraController&> query) {
        for (auto [controller] : query)
        {
            if (input.justPressed("camera-toggle"))
            {
                controller.enabled = !controller.enabled;
            }
        }
    });

    cubos.system("render")
        .after(gizmosDrawTag)
        .call([](Gizmos& gizmos, Query<const LocalToWorld&, const Collider&> query) {
            for (auto [localToWorld, collider] : query)
            {
                auto size = collider.localAABB.box().halfSize * 2.0F;
                glm::mat4 transform = glm::scale(localToWorld.mat * collider.transform, size);
                gizmos.color({1.0F, 1.0F, 1.0F});
                gizmos.drawWireBox("local AABB", transform);

                gizmos.color({1.0F, 0.0F, 0.0F});
                gizmos.drawWireBox("world AABB", collider.worldAABB.min(), collider.worldAABB.max());
            }
        });

    cubos.run();
    return 0;
}