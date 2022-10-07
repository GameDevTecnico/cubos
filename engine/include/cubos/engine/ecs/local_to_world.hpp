#ifndef CUBOS_ENGINE_ECS_LOCAL_TO_WORLD_HPP
#define CUBOS_ENGINE_ECS_LOCAL_TO_WORLD_HPP

#include <cubos/core/ecs/vec_storage.hpp>

#include <glm/glm.hpp>

namespace cubos::engine::ecs
{
    /// A matrix calculated from the position, rotation and scale of an entity.
    /// This matrix should
    struct LocalToWorld
    {
        using Storage = core::ecs::VecStorage<LocalToWorld>;

        glm::mat4 mat = glm::mat4(1.0f); ///< The local to world matrix.
    };

} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_LOCAL_TO_WORLD_HPP
