#ifndef CUBOS_ENGINE_ECS_ROTATION_HPP
#define CUBOS_ENGINE_ECS_ROTATION_HPP

#include <cubos/core/ecs/vec_storage.hpp>

#include <glm/gtx/quaternion.hpp>

namespace cubos::engine::ecs
{
    /// Assigns a rotation to an entity.
    struct Rotation
    {
        using Storage = core::ecs::VecStorage<Rotation>;

        glm::quat quat = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); ///< The rotation of the entity.
    };
} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_ROTATION_HPP
