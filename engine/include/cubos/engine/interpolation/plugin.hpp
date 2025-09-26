/// @dir
/// @brief @ref interpolation-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup interpolation-plugin

#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{

    /// @brief Systems with this tag run once if this is fixed step update.
    CUBOS_ENGINE_API extern Tag interpolationResetTag;

    /// @brief Component which holds the the previous and next frames position, rotation and scale for interpolation.
    /// @ingroup interpolation-plugin
    struct CUBOS_ENGINE_API Interpolated
    {
        CUBOS_REFLECT;

        glm::vec3 currentPosition; ///< Currently interpolated frame position.
        // glm::quat currentRotation; ///< Currently interpolated frame rotation.
        float currentScale; ///< Currently interpolated frame scale factor.

        glm::vec3 previousPosition; ///< Previous frame position.
        // glm::quat previousRotation; ///< Previous frame rotation.
        float previousScale; ///< Previous frame scale factor.

        glm::vec3 nextPosition; ///< Next frame position.
        // glm::quat nextRotation; ///< Next frame rotation.
        float nextScale; ///< Next frame scale factor.
    };

    /// @ingroup interpolation-plugin
    /// @brief Interpolates position, rotation and scale of entities with the @ref Interpolated component in between
    /// fixed updates.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup interpolation-plugin
    void interpolationPlugin(Cubos& cubos);
} // namespace cubos::engine