#include <cubos/engine/systems/render_system.hpp>

#include <cubos/core/ecs/world_view.hpp>
#include <cubos/engine/components/transform.hpp>
#include <cubos/engine/components/grid_renderable.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace cubos;
using namespace cubos::engine::components;
using namespace cubos::engine::systems;

void renderSystem(core::ecs::World &world, engine::gl::Renderer &renderer)
{
    for(auto entity : core::ecs::WorldView<Transform, GridRenderable>(world))
    {
        auto* transform = world.getComponent<Transform>(entity);
        auto* gridRenderable = world.getComponent<GridRenderable>(entity);

        glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), transform->position) * glm::toMat4(transform->rotation) *
                             glm::scale(glm::mat4(1.0f), transform->scale) * glm::translate(glm::mat4{1.0f}, gridRenderable->modelOffset);

        renderer.drawModel(gridRenderable->modelID, modelMat);
    }
}
