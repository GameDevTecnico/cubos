#include <glm/gtc/random.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/gl/debug.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/collisions/broad_phase/potentially_colliding_with.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::geom::Box;
using cubos::core::io::Key;
using cubos::core::io::Modifiers;

using namespace cubos::engine;

struct State
{
    bool collided = false;

    Entity a;
    Entity b;

    glm::vec3 aRotationAxis;
    glm::vec3 bRotationAxis;
};

int main()
{
    auto cubos = Cubos();

    cubos.addPlugin(collisionsPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(inputPlugin);

    cubos.addResource<State>();

    cubos.startupSystem("activate assets IO").tagged("cubos.settings").call([](Settings& settings) {
        settings.setBool("assets.io.enabled", false);
    });

    cubos.startupSystem("setup input").call([](Input& input) {
        // Add procedural asset for detecting a reset action on a space key press.
        auto bindings = InputBindings{};
        bindings.actions()["reset"].keys().emplace_back(Key::Space, Modifiers::None);
        input.bind(bindings);
    });

    cubos.startupSystem("setup camera").call([](Commands commands, ActiveCameras& cameras) {
        // Spawn the camera.
        cameras.entities[0] =
            commands.create()
                .add(Camera{.fovY = 60.0, .zNear = 0.1F, .zFar = 100.0F})
                .add(LocalToWorld{})
                .add(Position{{-4.0, 1.5, 0}})
                .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{3.0, -1.0, 0.0}), glm::vec3{0.0, 1.0, 0.0})})
                .entity();
    });

    cubos.startupSystem("create colliders").call([](State& state, Commands commands) {
        state.a = commands.create()
                      .add(Collider{})
                      .add(CapsuleCollisionShape{{0.5F, 1.0F}})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, -2.0F}})
                      .add(Rotation{})
                      .entity();
        state.aRotationAxis = glm::sphericalRand(1.0F);

        state.b = commands.create()
                      .add(Collider{})
                      .add(BoxCollisionShape{})
                      .add(LocalToWorld{})
                      .add(Position{glm::vec3{0.0F, 0.0F, 2.0F}})
                      .add(Rotation{})
                      .entity();
        state.bRotationAxis = glm::sphericalRand(1.0F);
    });

    cubos.system("move colliders")
        .before("cubos.transform.update")
        .call([](State& state, const Input& input, Query<Position&, Rotation&> query) {
            auto [aPos, aRot] = *query.at(state.a);
            auto [bPos, bRot] = *query.at(state.b);

            if (state.collided)
            {
                if (input.pressed("reset"))
                {
                    state.collided = false;

                    aPos.vec = glm::vec3{0.0F, 0.0F, -2.0F};
                    aRot.quat = glm::quat{1.0F, 0.0F, 0.0F, 0.0F};
                    state.aRotationAxis = glm::sphericalRand(1.0F);

                    bPos.vec = glm::vec3{0.0F, 0.0F, 2.0F};
                    bRot.quat = glm::quat{1.0F, 0.0F, 0.0F, 0.0F};
                    state.bRotationAxis = glm::sphericalRand(1.0F);
                }
                return;
            }

            aRot.quat = glm::rotate(aRot.quat, 0.01F, state.aRotationAxis);
            aPos.vec += glm::vec3{0.0F, 0.0F, 0.01F};

            bRot.quat = glm::rotate(bRot.quat, 0.01F, state.bRotationAxis);
            bPos.vec += glm::vec3{0.0F, 0.0F, -0.01F};
        });

    cubos.system("check collisions")
        .tagged("updated")
        .after("cubos.collisions.broad")
        .call([](Query<Entity, PotentiallyCollidingWith&, Entity> query, State& state) {
            for (auto [ent1, colliding, ent2] : query)
            {
                if ((ent1 == state.a && ent2 == state.b) || (ent1 == state.b && ent2 == state.a))
                {
                    state.collided = true;
                }
            }
        });

    cubos.system("render").after("updated").call([](Query<const LocalToWorld&, const Collider&> query) {
        for (auto [localToWorld, collider] : query)
        {
            cubos::core::gl::Debug::drawWireBox(
                collider.localAABB.box(),
                glm::translate(localToWorld.mat * collider.transform, collider.localAABB.center()));
            cubos::core::gl::Debug::drawWireBox(collider.worldAABB.box(),
                                                glm::translate(glm::mat4{1.0}, collider.worldAABB.center()),
                                                glm::vec3{1.0, 0.0, 0.0});
        }
    });

    cubos.run();
    return 0;
}