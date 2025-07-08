/// @file
/// @brief Component @ref cubos::engine::Interpolated.
/// @ingroup physics-solver-plugin

#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which holds the the previous and next frames position, rotation and scale for interpolation.
    /// @ingroup physics-solver-plugin
    struct CUBOS_ENGINE_API Interpolated
    {
        CUBOS_REFLECT;

        glm::vec3 previousPosition; ///< Previous frame position.
        glm::quat previousRotation; ///< Previous frame rotation.
        float previousScale;        ///< Previous frame scale factor.

        glm::vec3 nextPosition; ///< Next frame position.
        glm::quat nextRotation; ///< Next frame rotation.
        float nextScale;        ///< Next frame scale factor.
    };
} // namespace cubos::engine