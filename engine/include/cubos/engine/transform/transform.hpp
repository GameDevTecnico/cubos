/// @file
/// @brief Component @ref cubos::engine::Transform.
/// @ingroup transform-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the transformation matrix of an entity.
    ///
    /// @ingroup transform-plugin
    struct [[cubos::component("cubos/transform", VecStorage)]] Transform
    {
        CUBOS_REFLECT;

        glm::mat4 mat = glm::mat4(1.0F); ///< Local to world space matrix.

        /// @brief Get the transformation matrix from local to world space.
        /// @return The transformation matrix from local to world space.
        glm::mat4 localToWorld() const;

        /// @brief Get the position of the transform.
        /// @return The position of the transform.
        glm::vec3 position() const;

        /// @brief Set the position of the transform.
        /// @param pos The new position.
        void position(const glm::vec3& pos);

        /// @brief Translate the transform.
        /// @param delta The translation vector.
        void translate(const glm::vec3& delta);

        /// @brief Get the rotation of the transform.
        /// @return The rotation of the transform.
        glm::quat rotation() const;

        /// @brief Get the rotation of the transform as euler angles.
        /// @return The rotation of the transform as euler angles.
        glm::vec3 eulerRotation() const;

        /// @brief Set the rotation of the transform.
        /// @param rot The new rotation.
        void rotation(const glm::quat& rot);

        /// @brief Set the rotation of the transform.
        /// @param euler The new rotation as euler angles.
        void rotation(const glm::vec3& euler);

        /// @brief Rotate the transform.
        /// @param delta The rotation quaternion.
        void rotate(const glm::quat& delta);

        /// @brief Rotate the transform.
        /// @param eulerDelta The rotation as euler angles.
        void rotate(const glm::vec3& eulerDelta);

        /// @brief Get the scale of the transform.
        /// @return The scale of the transform.
        float scale() const;

        /// @brief Set the scale of the transform.
        /// @param scale The new scale.
        void scale(float scale);
    };
} // namespace cubos::engine
