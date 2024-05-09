/// @dir
/// @brief @ref ui-color-rect-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup ui-color-rect-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup ui-color-rect-plugin Canvas
    /// @ingroup engine
    /// @brief Adds color rectangle elements.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup ui-color-rect-plugin
    CUBOS_ENGINE_API void colorRectPlugin(Cubos& cubos);
} // namespace cubos::engine
