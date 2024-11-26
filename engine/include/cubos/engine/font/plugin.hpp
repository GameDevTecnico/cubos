/// @dir
/// @brief @ref font-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup font-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup font-plugin Font
    /// @ingroup engine
    /// @brief Adds fonts to @b Cubos using msdfgen.
    ///
    /// ## Bridges
    /// - @ref FontBridge - loads @ref Font assets.
    /// - @ref FontAtlasBridge - loads @ref FontAtlas assets.
    ///
    /// ## Dependencies
    /// - @ref assets-plugin
    /// - @ref window-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup font-plugin
    CUBOS_ENGINE_API void fontPlugin(Cubos& cubos);
} // namespace cubos::engine
