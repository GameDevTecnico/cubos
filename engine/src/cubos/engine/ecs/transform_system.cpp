#include <cubos/engine/ecs/transform_system.hpp>
#include <cubos/engine/ecs/position.hpp>
#include <cubos/engine/ecs/rotation.hpp>
#include <cubos/engine/ecs/scale.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace cubos::core::ecs;
using namespace cubos::engine::ecs;

void TransformSystem::init(World& world)
{
    world.registerComponent<LocalToWorld>();
    world.registerComponent<Position>();
    world.registerComponent<Rotation>();
    world.registerComponent<Scale>();
}

void TransformSystem::process(World& world, core::ecs::Entity entity, LocalToWorld& localToWorld)
{
    localToWorld.mat = glm::mat4(1.0f);
    if (world.hasComponent<Position>(entity))
        localToWorld.mat = glm::translate(localToWorld.mat, world.getComponent<Position>(entity).vec);
    if (world.hasComponent<Rotation>(entity))
        localToWorld.mat *= glm::toMat4(world.getComponent<Rotation>(entity).quat);
    if (world.hasComponent<Scale>(entity))
        localToWorld.mat = glm::scale(localToWorld.mat, glm::vec3(world.getComponent<Scale>(entity).factor));
}
