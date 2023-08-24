/// @dir
/// @brief Scene plugin directory.

/// @file
/// @brief Plugin entry point.

#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace cubos::engine
{
    /// @defgroup scene-plugin Scene
    /// @ingroup plugins
    /// @brief Adds scenes to CUBOS.
    ///
    /// ## Bridges
    /// - @ref SceneBridge - registered with the `.cubos` extension, loads @ref Scene assets.
    ///
    /// ## Dependencies
    /// - @ref assets-plugin

    /// @brief Plugin entry function.
    /// @param cubos CUBOS. main class.
    /// @ingroup scene-plugin
    void scenePlugin(Cubos& cubos);
} // namespace cubos::engine
