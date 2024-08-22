/// @file
/// @brief Component @ref cubos::engine::Inertia.
/// @ingroup physics-plugin

#pragma once

#include <glm/gtx/quaternion.hpp>
#include <glm/mat3x3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which defines the inertia of a body.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API Inertia
    {
        CUBOS_REFLECT;

        glm::mat3 inertia;
        glm::mat3 inverseInertia;
        bool autoUpdate = false;

        glm::mat3 rotatedInertia(const glm::quat& rotationQuat) const
        {
            auto rotationMat = glm::mat3(rotationQuat);
            return rotationMat * inertia * glm::transpose(rotationMat);
        }

        // TODO: check if we use this many times. If we do, it might be worth it to calculate and store them
        // somewhere instead.
        glm::mat3 rotatedInverseInertia(const glm::quat& rotationQuat) const
        {
            auto rotationMat = glm::mat3(rotationQuat);
            return rotationMat * inverseInertia * glm::transpose(rotationMat);
        }
    };
} // namespace cubos::engine
