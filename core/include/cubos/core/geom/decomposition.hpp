/// @file
/// @brief Utility functions to decompose transformation matrices.
/// @ingroup core-geom
///
/// @details These functions are limited to matrices with uniform scale and no shear.

#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#pragma once

namespace cubos::core::geom
{
    /// @brief Get the position of a transformation matrix.
    /// @param mat The transformation matrix.
    /// @return The position of the transformation matrix.
    static glm::vec3 position(const glm::mat4& mat);

    /// @brief Set the position of a transformation matrix.
    /// @param mat The transformation matrix.
    /// @param pos The new position.
    static void position(glm::mat4& mat, const glm::vec3& pos);

    /// @brief Translate a transformation matrix.
    /// @param mat The transformation matrix.
    /// @param delta The translation vector.
    static void translate(glm::mat4& mat, const glm::vec3& delta);

    /// @brief Get the rotation of a transformation matrix.
    /// @param mat The transformation matrix.
    /// @return The rotation of the transformation matrix.
    static glm::quat rotation(const glm::mat4& mat);

    /// @brief Get the rotation of a transformation matrix as euler angles.
    /// @param mat The transformation matrix.
    /// @return The rotation of the transformation matrix as euler angles.
    static glm::vec3 eulerRotation(const glm::mat4& mat);

    /// @brief Set the rotation of a transformation matrix.
    /// @param mat The transformation matrix.
    /// @param rot The new rotation.
    static void rotation(glm::mat4& mat, const glm::quat& rot);

    /// @brief Set the rotation of a transformation matrix.
    /// @param mat The transformation matrix.
    /// @param euler The new rotation as euler angles.
    static void rotation(glm::mat4& mat, const glm::vec3& euler);

    /// @brief Rotate a transformation matrix.
    /// @param mat The transformation matrix.
    /// @param delta The rotation quaternion.
    static void rotate(glm::mat4& mat, const glm::quat& delta);

    /// @brief Rotate a transformation matrix.
    /// @param mat The transformation matrix.
    /// @param eulerDelta The rotation as euler angles.
    static void rotate(glm::mat4& mat, const glm::vec3& eulerDelta);

    /// @brief Get the scale of a transformation matrix.
    /// @param mat The transformation matrix.
    /// @return The scale of the transformation matrix.
    static float scale(const glm::mat4& mat);

    /// @brief Set the scale of a transformation matrix.
    /// @param mat The transformation matrix.
    /// @param scale The new scale.
    static void scale(glm::mat4& mat, float scale);
} // namespace cubos::core::geom