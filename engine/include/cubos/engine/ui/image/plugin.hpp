/// @dir
/// @brief @ref ui-image-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup ui-image-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup ui-image-plugin Color Rect
    /// @ingroup ui-plugins
    /// @brief Adds image element to UI.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup ui-image-plugin
    CUBOS_ENGINE_API void uiImagePlugin(Cubos& cubos);
} // namespace cubos::engine
