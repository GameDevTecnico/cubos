/// @dir
/// @brief @ref scene-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup scene-plugin

#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace cubos::engine
{
    /// @defgroup scene-plugin Scene
    /// @ingroup engine
    /// @brief Adds scenes to @b CUBOS.
    /// @see Take a look at the @ref examples-engine-scene example for a demonstration of this
    /// plugin.
    ///
    /// ## Bridges
    /// - @ref SceneBridge - registered with the `.cubos` extension, loads @ref Scene assets.
    ///
    /// ## Dependencies
    /// - @ref assets-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup scene-plugin
    CUBOS_ENGINE_API void scenePlugin(Cubos& cubos);
} // namespace cubos::engine
