#include <cubos/engine/ecs/draw_system.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace cubos::core::ecs;
using namespace cubos::engine::ecs;

void DrawSystem::process(World& world, uint64_t entity, Grid& grid, LocalToWorld& localToWorld)
{
    this->frame.draw(grid.asset->id, localToWorld.mat * glm::translate(glm::mat4(1.0f), grid.modelOffset));
}
