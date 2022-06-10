#ifndef CUBOS_ENGINE_ECS_GRID_HPP
#define CUBOS_ENGINE_ECS_GRID_HPP

#include <cubos/core/ecs/vec_storage.hpp>

#include <cubos/engine/data/grid.hpp>
#include <cubos/engine/data/asset.hpp>

#include <glm/glm.hpp>

namespace cubos::engine::ecs
{
    /// A Grid component which makes the renderer render a grid.
    struct Grid
    {
        using Storage = core::ecs::VecStorage<Grid>;

        data::Asset<data::Grid> asset; ///< The asset of the grid being rendered.
        glm::vec3 modelOffset;         ///< The offset of the model.
    };

} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_GRID_HPP
