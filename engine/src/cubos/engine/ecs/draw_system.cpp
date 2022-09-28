#include <cubos/engine/ecs/draw_system.hpp>

#include <glm/gtc/matrix_transform.hpp>

void cubos::engine::ecs::drawSystem(gl::Frame& frame, core::ecs::Query<const Grid&, const LocalToWorld&> query)
{
    for (auto [entity, grid, localToWorld] : query)
    {
        frame.draw(grid.handle, localToWorld.mat * glm::translate(glm::mat4(1.0f), grid.modelOffset));
    }
}
