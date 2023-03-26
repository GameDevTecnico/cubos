#pragma once

#include <glm/glm.hpp>

namespace cubos::engine::ecs
{
    /// Assigns a uniform scale to an entity.
    struct [[cubos::component("cubos/scale", VecStorage)]] Scale
    {
        float factor; ///< The scale factor of the entity.
    };
} // namespace cubos::engine::ecs
