#ifndef CUBOS_ENGINE_ECS_GRID_HPP
#define CUBOS_ENGINE_ECS_GRID_HPP

#include <components/base.hpp>

#include <cubos/engine/data/asset.hpp>
#include <cubos/engine/data/grid.hpp>

#include <glm/glm.hpp>
#include <string>

namespace cubos::engine::ecs
{
    /// A Grid component which makes the renderer render a grid.
    struct [[cubos::component("cubos/grid", VecStorage)]] Grid
    {
        data::Asset<data::Grid> handle; ///< The handle of the grid being rendered.
        glm::vec3 modelOffset;          ///< The offset of the model.
    };

} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_GRID_HPP
