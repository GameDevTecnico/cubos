#pragma once

#include <glm/glm.hpp>

namespace cubos::engine::ecs
{
    /// Assigns a position to an entity.
    struct [[cubos::component("cubos/position", VecStorage)]] Position
    {
        glm::vec3 vec = {0.0F, 0.0F, 0.0F}; ///< The position of the entity.
    };
} // namespace cubos::engine::ecs
