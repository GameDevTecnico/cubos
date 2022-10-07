#ifndef CUBOS_ENGINE_ECS_GRID_HPP
#define CUBOS_ENGINE_ECS_GRID_HPP

#include <cubos/core/ecs/vec_storage.hpp>

#include <cubos/engine/gl/renderer.hpp>

#include <glm/glm.hpp>

namespace cubos::engine::ecs
{
    /// A Grid component which makes the renderer render a grid.
    struct Grid
    {
        using Storage = core::ecs::VecStorage<Grid>;

        gl::RendererGrid handle; ///< The handle of the grid being rendered.
        glm::vec3 modelOffset;   ///< The offset of the model.
    };

} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_GRID_HPP
