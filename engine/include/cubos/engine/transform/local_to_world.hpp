/// @file
/// @brief Component @ref cubos::engine::LocalToWorld.
/// @ingroup transform-plugin

#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the transformation matrix of an entity, from local to world
    /// space.
    ///
    /// @note This component is written to by the @ref transform-plugin "transform plugin", thus it never makes sense to
    /// write to it manually.
    ///
    /// @sa The value is calculated from the @ref LocalToParent component and the parent's @ref LocalToWorld, if there's
    /// any.
    /// @ingroup transform-plugin
    struct CUBOS_ENGINE_API LocalToWorld
    {
        CUBOS_REFLECT;

        glm::mat4 mat = glm::mat4(1.0F); ///< Local to world space matrix.

        /// @brief Gets Local to world space matrix reversed.
        /// @return World to local space matrix inversed.
        glm::mat4 inverse() const;

        /// @brief Gets global position of the entity.
        /// @return Position vector in world space.
        glm::vec3 worldPosition() const;

        /// @brief Gets global rotation of the entity.
        /// @return Rotation quaternion in world space.
        glm::quat worldRotation() const;

        /// @brief Gets global scale of the entity.
        /// @return Scale value in world space.
        float worldScale() const;
    };

} // namespace cubos::engine
