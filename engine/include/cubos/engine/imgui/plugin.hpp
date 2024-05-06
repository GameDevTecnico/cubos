/// @dir
/// @brief @ref imgui-plugin ImGui plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup imgui-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup imgui-plugin ImGui integration
    /// @ingroup engine
    /// @brief Initializes and configures ImGui for @b CUBOS.
    ///
    /// @note Systems using `ImGui` **must be** tagged with `cubos.imgui`.
    ///
    /// ## Resources
    /// - @ref DataInspector - used to inspect/modify objects via UI.
    ///
    /// ## Dependencies
    /// - @ref settings-plugin
    /// - @ref window-plugin
    /// - @ref render-target-plugin

    /// @brief `ImGui` is initialized, after @ref windowInitTag.
    CUBOS_ENGINE_API extern Tag imguiInitTag;

    /// @brief The ImGui frame begins, after @ref windowPollTag.
    CUBOS_ENGINE_API extern Tag imguiBeginTag;

    /// @brief The ImGui frame ends and is rendered to the window's render target.
    CUBOS_ENGINE_API extern Tag imguiEndTag;

    /// @brief Tag to be used by all systems which call ImGui functions. Runs between @ref imguiBeginTag and @ref
    /// imguiEndTag.
    CUBOS_ENGINE_API extern Tag imguiTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup imgui-plugin
    CUBOS_ENGINE_API void imguiPlugin(Cubos& cubos);
} // namespace cubos::engine
