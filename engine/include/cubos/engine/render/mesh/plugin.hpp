/// @dir
/// @brief @ref render-mesh-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-mesh-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-mesh-plugin Mesh
    /// @ingroup render-plugins
    /// @brief Adds and manages @ref RenderMesh components.
    ///
    /// ## Settings
    /// - `renderMeshPool.bucketCount` - number of buckets in the mesh pool (default: `1024`).
    /// - `renderMeshPool.bucketSize` - maximum face count per bucket (default: `1024`).
    ///
    /// ## Dependencies
    /// - @ref settings-plugin
    /// - @ref window-plugin
    /// - @ref assets-plugin
    /// - @ref render-voxels-plugin

    /// @brief Render mesh pool is initialized (startup).
    CUBOS_ENGINE_API extern Tag renderMeshPoolInitTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-mesh-plugin
    CUBOS_ENGINE_API void renderMeshPlugin(Cubos& cubos);
} // namespace cubos::engine
