/// @dir
/// @brief @ref color-rect-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup color-rect-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup color-rect-plugin Canvas
    /// @ingroup engine
    /// @brief Adds color rect.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup color-rect-plugin
    CUBOS_ENGINE_API void colorRectPlugin(Cubos& cubos);
} // namespace cubos::engine
