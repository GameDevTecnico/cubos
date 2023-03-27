#ifndef CUBOS_ENGINE_ECS_POSITION_HPP
#define CUBOS_ENGINE_ECS_POSITION_HPP

#include <glm/glm.hpp>

namespace cubos::engine::ecs
{
    /// Assigns a position to an entity.
    struct [[cubos::component("cubos/position", VecStorage)]] Position
    {
        glm::vec3 vec = {0.0f, 0.0f, 0.0f}; ///< The position of the entity.
    };
} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_POSITION_HPP
