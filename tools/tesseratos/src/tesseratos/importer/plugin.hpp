/// @dir
/// @brief @ref tesseratos-importer-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-importer-plugin

#pragma once

#include <cubos/engine/assets/plugin.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-importer-plugin Asset explorer
    /// @ingroup tesseratos
    /// @brief Allows importing files such as .qb through a ImGui window.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin
    /// - @ref assets-plugin
    /// - @ref tesseratos-toolbox-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-importer-plugin
    void importerPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
