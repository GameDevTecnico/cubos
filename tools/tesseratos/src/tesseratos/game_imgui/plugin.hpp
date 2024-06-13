/// @dir
/// @brief @ref tesseratos-game-imgui-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-game-imgui-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-game-imgui-plugin Game ImGui
    /// @ingroup tesseratos
    /// @brief Adds a ImGui context switch function to the @ref Game, allowing for ImGui to be used within it.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-game-imgui-plugin
    void gameImGuiPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
