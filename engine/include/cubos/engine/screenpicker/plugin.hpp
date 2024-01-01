/// @dir
/// @brief @ref screenpicker-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup screenpicker-plugin

#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/screenpicker/screenpicker.hpp>

namespace cubos::engine
{
    /// @defgroup screenpicker-plugin ScreenPicker
    /// @ingroup engine
    /// @brief Used to select entities and gizmos by clicking them.
    ///
    /// ## Resources
    /// - @ref ScreenPicker - provides a texture to store entity and gizmo ids.
    ///
    /// ## Startup tags
    /// - `cubos.screenpicker.init` - the ScreenPicker resource is initialized, after `cubos.window.init`
    ///
    /// ## Tags
    /// - `cubos.screenpicker.clear` - the picking texture is cleared
    /// - `cubos.screenpicker.resize` - window resize events are handled and the ScreenPicker texture is resized, after
    /// `cubos.window.poll` and before `cubos.screenpicker.clear`.
    ///
    /// ## Dependencies
    /// - @ref window-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup screenpicker-plugin
    void screenPickerPlugin(Cubos& cubos);
} // namespace cubos::engine
