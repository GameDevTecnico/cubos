#include <glm/gtc/random.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/assets/plugin.hpp>
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

#include "../src/collisions/broad_phase/collider_aabb.hpp"

using cubos::core::geom::Box;
using cubos::core::io::Key;
using cubos::core::io::Modifiers;

using namespace cubos::engine;

static const Asset<InputBindings> BindingsAsset = AnyAsset("53d85d6f-42a8-4eb8-aeae-1754a4cb20df");

struct Options
{
    CUBOS_ANONYMOUS_REFLECT(Options);

    bool fixedDistance = false;
    enum Scenarios
    {
        Circle,
        Pendulum,
        DoublePendulum,
        PushPull
    };
    Scenarios currentScenario = DoublePendulum;
};

struct State
{
    CUBOS_ANONYMOUS_REFLECT(State);

    Entity a;
    Entity b;
    Entity c;
};

void createScenario(Commands& commands, State& state, Options& options)
{
    if (options.currentScenario == Options::Scenarios::Circle)
    {
        state.a = commands.create()
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, -2.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 10000000000000000.0F, .velocity = {0.0F, 0.0F, 0.0F}})
                      .entity();

        state.b = commands.create()
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 2.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {1.0F, 0.0F, 1.0F}})
                      .entity();

        commands.relate(state.a, state.b,
                        DistanceConstraint{.isRigid = false,
                                           .minDistance = 0.0F,
                                           .maxDistance = 5.0F,
                                           .localAnchor1 = {0.0F, 0.0F, 0.0F},
                                           .localAnchor2 = {0.0F, 0.0F, 0.0F}});
    }
    if (options.currentScenario == Options::Scenarios::Pendulum)
    {
        state.a = commands.create()
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 0.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 10000000000000000.0F, .velocity = {0.0F, 0.0F, 0.0F}})
                      .entity();

        state.b = commands.create()
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 5.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {0.0F, 0.0F, 0.0F}})
                      .entity();

        commands.relate(state.a, state.b,
                        DistanceConstraint{.isRigid = false,
                                           .minDistance = 0.0F,
                                           .maxDistance = 5.0F,
                                           .localAnchor1 = {0.0F, 0.0F, 0.0F},
                                           .localAnchor2 = {0.0F, 0.0F, 0.0F}});
    }
    if (options.currentScenario == Options::Scenarios::DoublePendulum)
    {
        state.a = commands.create()
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 0.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 10000000000000000.0F, .velocity = {0.0F, 0.0F, 0.0F}})
                      .entity();

        state.b = commands.create()
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 5.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {0.0F, 0.0F, 0.0F}})
                      .entity();

        state.c = commands.create()
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 10.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {0.0F, 0.0F, 0.0F}})
                      .entity();

        commands.relate(state.a, state.b,
                        DistanceConstraint{.isRigid = false,
                                           .minDistance = 3.0F,
                                           .maxDistance = 5.0F,
                                           .localAnchor1 = {0.0F, 0.0F, 0.0F},
                                           .localAnchor2 = {0.0F, 0.4F, 0.4F}});

        commands.relate(state.b, state.c,
                        DistanceConstraint{.isRigid = false,
                                           .minDistance = 3.0F,
                                           .maxDistance = 5.0F,
                                           .localAnchor1 = {0.0F, -0.4F, -0.4F},
                                           .localAnchor2 = {0.0F, 0.0F, 0.0F}});
    }
    if (options.currentScenario == Options::Scenarios::PushPull)
    {
        state.a = commands.create()
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 0.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 10000000000000000.0F, .velocity = {0.0F, 0.0F, 0.0F}})
                      .entity();

        state.b = commands.create()
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, -5.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {0.0F, 0.0F, 0.0F}})
                      .entity();

        state.c = commands.create()
                      .add(BoxCollisionShape{})
                      .add(CollisionLayers{})
                      .add(CollisionMask{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, -15.0F}})
                      .add(Rotation{})
                      .add(PhysicsBundle{.mass = 500.0F, .velocity = {0.0F, 0.0F, 5.0F}})
                      .entity();

        commands.relate(state.a, state.b,
                        DistanceConstraint{.isRigid = false,
                                           .minDistance = 3.0F,
                                           .maxDistance = 5.0F,
                                           .localAnchor1 = {0.0F, 0.0F, 0.0F},
                                           .localAnchor2 = {0.0F, 0.0F, 0.0F}});
    }
}

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

    cubos.startupSystem("create starting scenario").call([](State& state, Options& options, Commands commands) {
        createScenario(commands, state, options);
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
        .call([](Gizmos& gizmos, Query<const LocalToWorld&, const ColliderAABB&> query,
                 Query<const LocalToWorld&, DistanceConstraint&, const LocalToWorld&> query2) {
            for (auto [localToWorld, colliderAABB] : query)
            {
                auto size = colliderAABB.localAABB.box().halfSize * 2.0F;
                glm::mat4 transform = glm::scale(localToWorld.mat, size);
                gizmos.color({1.0F, 1.0F, 1.0F});
                gizmos.drawWireBox("local AABB", transform);

                gizmos.color({1.0F, 0.0F, 0.0F});
                gizmos.drawWireBox("world AABB", colliderAABB.worldAABB.min(), colliderAABB.worldAABB.max());
            }
            // draw the constraints
            for (auto [localToWorld1, constraint, localToWorld2] : query2)
            {
                glm::vec3 pointFrom = (localToWorld1.mat * glm::vec4(constraint.localAnchor1, 1.0F));
                glm::vec3 direction =
                    glm::normalize(glm::vec3(localToWorld2.mat * glm::vec4(constraint.localAnchor2, 1.0F)) - pointFrom);

                gizmos.color({1.0F, 0.0F, 0.0F});
                gizmos.drawLine("the line between", pointFrom,
                                constraint.isRigid ? pointFrom + glm::normalize(direction) * constraint.fixedDistance
                                                   : pointFrom + glm::normalize(direction) * constraint.minDistance);
                if (!constraint.isRigid)
                {
                    gizmos.color({0.0F, 0.0F, 1.0F});
                    gizmos.drawLine("the line between", pointFrom + glm::normalize(direction) * constraint.minDistance,
                                    pointFrom + glm::normalize(direction) * constraint.maxDistance);
                }
            }
        });

    cubos.system("apply gravity")
        .tagged(physicsApplyForcesTag)
        .call([](State& state, Options& options, Query<Velocity&, Force&, Mass&> query) {
            if (options.currentScenario == Options::Scenarios::Circle ||
                options.currentScenario == Options::Scenarios::PushPull)
            {
                return;
            }

            auto [bVel, bFor, bMass] = *query.at(state.b);

            // Apply gravity force
            glm::vec3 gravitationForce = bMass.mass * glm::vec3(0.0F, -2.0F, 0.0F);
            bFor.add(gravitationForce);

            if (options.currentScenario == Options::Scenarios::DoublePendulum)
            {
                auto [cVel, cFor, cMass] = *query.at(state.c);

                cFor.add(gravitationForce);
            }
        });

    cubos.system("switch scenarios").call([](State& state, Options& options, Commands commands, const Input& input) {
        if (input.justPressed("reset"))
        {
            commands.destroy(state.a);
            commands.destroy(state.b);
            if (!state.c.isNull())
            {
                commands.destroy(state.c);
            }
            switch (options.currentScenario)
            {
            case Options::Scenarios::Circle:
                options.currentScenario = Options::Scenarios::Pendulum;
                break;
            case Options::Scenarios::Pendulum:
                options.currentScenario = Options::Scenarios::DoublePendulum;
                break;
            case Options::Scenarios::DoublePendulum:
                options.currentScenario = Options::Scenarios::PushPull;
                break;
            case Options::Scenarios::PushPull:
                options.currentScenario = Options::Scenarios::Circle;
                break;
            }
            createScenario(commands, state, options);
        }
    });

    cubos.run();
    return 0;
}
