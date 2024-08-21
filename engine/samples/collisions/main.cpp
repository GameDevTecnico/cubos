#include <glm/gtc/random.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/contact_manifold.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/gizmos/target.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/perspective_camera.hpp>
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

static CUBOS_DEFINE_TAG(collisionsSampleUpdated);

static const Asset<InputBindings> BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a331d7842e5");

struct DebugDraw
{
    CUBOS_ANONYMOUS_REFLECT(DebugDraw);

    bool normal = true;
    bool points = true;
    bool manifoldPolygon = true;
};

struct Options
{
    CUBOS_ANONYMOUS_REFLECT(Options);

    bool stopOnContact = false;
    bool rotate = true;
};

struct State
{
    CUBOS_ANONYMOUS_REFLECT(State);

    bool collided = false;

    Entity a;
    Entity b;

    glm::vec3 aRotationAxis;
    glm::vec3 bRotationAxis;
};

int main()
{
    auto cubos = Cubos();

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
    cubos.resource<DebugDraw>();
    cubos.resource<Options>();

    cubos.startupSystem("configure Assets").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
    });

    cubos.startupSystem("setup input").tagged(assetsTag).call([](const Assets& assets, Input& input) {
        auto bindings = assets.read<InputBindings>(BindingsAsset);
        input.bind(*bindings);
    });

    cubos.startupSystem("setup camera").call([](Commands commands) {
        auto targetEnt = commands.create().add(RenderTargetDefaults{}).add(GizmosTarget{}).entity();

        commands.create()
            .relatedTo(targetEnt, DrawsTo{})
            .add(PerspectiveCamera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
            .add(LocalToWorld{})
            .add(Position{{-4.0F, 1.5F, 0.0F}})
            .add(Rotation{Rotation::lookingAt({3.0F, -1.0F, 0.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
            .add(FreeCameraController{.phi = -15.0F, .theta = 90.0F});
    });

    cubos.startupSystem("create colliders").call([](State& state, Commands commands) {
        state.a = commands.create()
                      .add(Collider{})
                      .add(BoxCollisionShape{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, -2.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {0.0F, 0.0F, 1.0F}})
                      .entity();
        state.aRotationAxis = glm::sphericalRand(1.0F);

        state.b = commands.create()
                      .add(Collider{})
                      .add(BoxCollisionShape{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 2.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {0.0F, 0.0F, -1.0F}})
                      .entity();
        state.bRotationAxis = glm::sphericalRand(1.0F);
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

    cubos.system("move colliders")
        .tagged(physicsApplyForcesTag)
        .before(transformUpdateTag)
        .call(
            [](State& state, const Options& options, const Input& input, Query<Position&, Rotation&, Velocity&> query) {
                auto [aPos, aRot, aVel] = *query.at(state.a);
                auto [bPos, bRot, bVel] = *query.at(state.b);

                if (state.collided)
                {
                    if (options.stopOnContact)
                    {
                        aVel.vec = glm::vec3{0.0F, 0.0F, 0.0F};
                        bVel.vec = glm::vec3{0.0F, 0.0F, 0.0F};
                    }
                    if (input.pressed("reset"))
                    {
                        state.collided = false;

                        aPos.vec = glm::vec3{0.0F, 0.0F, -2.0F};
                        aRot.quat = glm::quat{1.0F, 0.0F, 0.0F, 0.0F};
                        aVel.vec = glm::vec3{0.0F, 0.0F, 1.0F};
                        state.aRotationAxis = glm::sphericalRand(1.0F);

                        bPos.vec = glm::vec3{0.0F, 0.0F, 2.0F};
                        bRot.quat = glm::quat{1.0F, 0.0F, 0.0F, 0.0F};
                        bVel.vec = glm::vec3{0.0F, 0.0F, -1.0F};
                        state.bRotationAxis = glm::sphericalRand(1.0F);
                    }
                }

                if (!(state.collided && options.stopOnContact))
                {
                    if (options.rotate)
                    {
                        aRot.quat = glm::rotate(aRot.quat, 0.001F, state.aRotationAxis);
                        bRot.quat = glm::rotate(bRot.quat, 0.001F, state.bRotationAxis);
                    }

                    aVel.vec += glm::vec3{0.0F, 0.0F, 0.01F};
                    bVel.vec -= glm::vec3{0.0F, 0.0F, 0.01F};
                }
            });

    cubos.tag(collisionsSampleUpdated);

    cubos.system("check collisions")
        .tagged(collisionsSampleUpdated)
        .after(cubos::engine::collisionsTag)
        .call([](Query<Entity, CollidingWith&, Entity> query, State& state) {
            for (auto [ent1, colliding, ent2] : query)
            {
                if ((ent1 == state.a && ent2 == state.b) || (ent1 == state.b && ent2 == state.a))
                {
                    state.collided = true;
                }
            }
        });

    cubos.system("render")
        .after(collisionsSampleUpdated)
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

    cubos.system("render collision manifolds")
        .after(collisionsTag)
        .after(gizmosDrawTag)
        .call([](Gizmos& gizmos, const DebugDraw& draw,
                 Query<Entity, const Position&, const ContactManifold&, Entity, const Position&> query) {
            for (auto [ent1, pos1, manifold, ent2, pos2] : query)
            {
                if (draw.normal)
                {
                    glm::vec3 origin = ent1 == manifold.entity ? pos1.vec : pos2.vec;
                    gizmos.color({0.0F, 1.0F, 0.0F});
                    gizmos.drawArrow("arrow", origin, manifold.normal, 0.01F, 0.05F, 0.7F, 0.05F, Gizmos::Space::World);
                }

                if (draw.manifoldPolygon && manifold.points.size() > 1)
                {
                    cubos::engine::ContactPointData start = manifold.points.back();
                    for (const cubos::engine::ContactPointData& end : manifold.points)
                    {
                        gizmos.color({1.0F, 1.0F, 0.0F});
                        gizmos.drawArrow("line", start.position1, end.position1 - start.position1, 0.01F, 0.05F, 1.0F,
                                         0.05F, Gizmos::Space::World);
                        start = end;
                    }
                }

                if (draw.points)
                {
                    for (auto point : manifold.points)
                    {
                        gizmos.color({0.0F, 0.0F, 1.0F});
                        gizmos.drawArrow("point", point.position1, glm::vec3(0.02F, 0.02F, 0.02F), 0.03F, 0.05F, 1.0F,
                                         0.05F, Gizmos::Space::World);

                        gizmos.color({1.0F, 0.0F, 1.0F});
                        gizmos.drawArrow("point", point.position2, glm::vec3(0.02F, 0.02F, 0.02F), 0.03F, 0.05F, 1.0F,
                                         0.05F, Gizmos::Space::World);
                    }
                }
            }
        });

    cubos.run();
    return 0;
}
