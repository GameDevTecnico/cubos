#ifndef CUBOS_ENGINE_ECS_POSITION_HPP
#define CUBOS_ENGINE_ECS_POSITION_HPP

#include <cubos/core/ecs/vec_storage.hpp>

#include <glm/glm.hpp>

namespace cubos::engine::ecs
{
    /// Assigns a position to an entity.
    struct Position
    {
        using Storage = core::ecs::VecStorage<Position>;

        glm::vec3 vec = {0.0f, 0.0f, 0.0f}; ///< The position of the entity.
    };

} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_POSITION_HPP
