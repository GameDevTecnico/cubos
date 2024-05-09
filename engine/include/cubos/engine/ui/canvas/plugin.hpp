/// @dir
/// @brief @ref canvas-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup canvas-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup canvas-plugin Canvas
    /// @ingroup engine
    /// @brief Adds UI canvas and elements.

    CUBOS_ENGINE_API extern Tag elementVPUpdateTag;
    CUBOS_ENGINE_API extern Tag elementPropagateTag;
    CUBOS_ENGINE_API extern Tag uiBeginTag;
    CUBOS_ENGINE_API extern Tag uiDrawTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup canvas-plugin
    CUBOS_ENGINE_API void canvasPlugin(Cubos& cubos);
} // namespace cubos::engine
