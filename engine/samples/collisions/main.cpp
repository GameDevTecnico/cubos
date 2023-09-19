#include <glm/gtc/random.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/gl/debug.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/collisions/aabb.hpp>
#include <cubos/engine/collisions/broad_phase_collisions.hpp>
#include <cubos/engine/collisions/colliders/box.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
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

static void settings(Write<Settings> settings)
{
    settings->setBool("assets.io.enabled", false);
}

static void init(Commands commands, Write<Input> input, Write<ActiveCameras> camera)
{
    // Add procedural asset for detecting a reset action on a space key press.
    auto bindings = InputBindings{};
    bindings.actions()["reset"].keys().push_back({Key::Space, Modifiers::None});
    input->bind(bindings);

    // Spawn the camera.
    camera->entities[0] =
        commands.create()
            .add(Camera{.fovY = 60.0, .zNear = 0.1F, .zFar = 100.0F})
            .add(LocalToWorld{})
            .add(Position{{-4.0, 1.5, 0}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{3.0, -1.0, 0.0}), glm::vec3{0.0, 1.0, 0.0})})
            .entity();
}

static void addColliders(Write<State> state, Commands commands)
{
    state->a = commands.create()
                   .add(BoxCollider{})
                   .add(LocalToWorld{})
                   .add(Position{glm::vec3{0.0F, 0.0F, -2.0F}})
                   .add(Rotation{})
                   .entity();
    state->aRotationAxis = glm::sphericalRand(1.0F);

    state->b = commands.create()
                   .add(BoxCollider{})
                   .add(LocalToWorld{})
                   .add(Position{glm::vec3{0.0F, 0.0F, 2.0F}})
                   .add(Rotation{})
                   .entity();
    state->bRotationAxis = glm::sphericalRand(1.0F);
}

static void updateTransform(Write<State> state, Read<Input> input, Query<Write<Position>, Write<Rotation>> query)
{
    auto [aPos, aRot] = query[state->a].value();
    auto [bPos, bRot] = query[state->b].value();

    if (state->collided)
    {
        if (input->pressed("reset"))
        {
            state->collided = false;

            aPos->vec = glm::vec3{0.0F, 0.0F, -2.0F};
            aRot->quat = glm::quat{1.0F, 0.0F, 0.0F, 0.0F};
            state->aRotationAxis = glm::sphericalRand(1.0F);

            bPos->vec = glm::vec3{0.0F, 0.0F, 2.0F};
            bRot->quat = glm::quat{1.0F, 0.0F, 0.0F, 0.0F};
            state->bRotationAxis = glm::sphericalRand(1.0F);
        }
        return;
    }

    aRot->quat = glm::rotate(aRot->quat, 0.01F, state->aRotationAxis);
    aPos->vec += glm::vec3{0.0F, 0.0F, 0.01F};

    bRot->quat = glm::rotate(bRot->quat, 0.01F, state->bRotationAxis);
    bPos->vec += glm::vec3{0.0F, 0.0F, -0.01F};
}

static void updateCollided(Query<Read<BoxCollider>> query, Write<State> state, Read<BroadPhaseCollisions> collisions)
{
    for (auto [entity, collider] : query)
    {
        for (auto& [collider1, collider2] : collisions->candidates(BroadPhaseCollisions::CollisionType::BoxBox))
        {
            if (collider1 == entity || collider2 == entity)
            {
                state->collided = true;
                break;
            }
        }
    }
}

static void render(Query<Read<LocalToWorld>, Read<BoxCollider>, Read<ColliderAABB>> query)
{
    for (auto [entity, localToWorld, collider, aabb] : query)
    {
        cubos::core::gl::Debug::drawWireBox(collider->shape, localToWorld->mat * collider->transform);
        cubos::core::gl::Debug::drawWireBox(aabb->box(), glm::translate(glm::mat4{1.0}, aabb->center()),
                                            glm::vec3{1.0, 0.0, 0.0});
    }
}

int main()
{
    auto cubos = Cubos();

    cubos.addPlugin(collisionsPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(inputPlugin);

    cubos.addResource<State>();

    cubos.startupSystem(settings).tagged("cubos.settings");
    cubos.startupSystem(init);

    cubos.startupSystem(addColliders);

    cubos.system(updateTransform).before("cubos.transform.update");
    cubos.system(updateCollided).tagged("updated").after("cubos.collisions");
    cubos.system(render).after("updated");

    cubos.run();
    return 0;
}