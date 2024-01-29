/// @dir
/// @brief @ref screen-picker-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup screen-picker-plugin

#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/screen_picker/screen_picker.hpp>

namespace cubos::engine
{
    /// @defgroup screen-picker-plugin ScreenPicker
    /// @ingroup engine
    /// @brief Used to select entities and gizmos by clicking them.
    ///
    /// ## Resources
    /// - @ref ScreenPicker - provides a texture to store entity and gizmo ids.
    ///
    /// ## Startup tags
    /// - `cubos.screenPicker.init` - the ScreenPicker resource is initialized, after `cubos.window.init`
    ///
    /// ## Tags
    /// - `cubos.screenPicker.clear` - the picking texture is cleared
    /// - `cubos.screenPicker.resize` - window resize events are handled and the ScreenPicker texture is resized, after
    /// `cubos.window.poll` and before `cubos.screenPicker.clear`.
    ///
    /// ## Dependencies
    /// - @ref window-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup screen-picker-plugin
    void screenPickerPlugin(Cubos& cubos);
} // namespace cubos::engine
