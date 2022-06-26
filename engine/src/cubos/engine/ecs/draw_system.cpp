#include <cubos/engine/ecs/draw_system.hpp>

#include <glm/gtc/matrix_transform.hpp>

using namespace cubos::core::ecs;
using namespace cubos::engine::ecs;

DrawSystem::DrawSystem(gl::Frame& frame) : frame(frame)
{
}

void DrawSystem::process(World& world, core::ecs::Entity entity, Grid& grid, LocalToWorld& localToWorld)
{
    this->frame.draw(grid.handle, localToWorld.mat * glm::translate(glm::mat4(1.0f), grid.modelOffset));
}
