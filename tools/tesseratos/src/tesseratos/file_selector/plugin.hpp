/// @dir
/// @brief @ref tesseratos-file-selector-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-file-selector-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-file-selector-plugin Asset explorer
    /// @ingroup tesseratos
    /// @brief Allows selecting files in the filesystem
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-file-selector-plugin
    void fileSelectorPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
