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
    /// ## Dependencies
    /// - @ref window-plugin

    /// @brief The ScreenPicker resource is initialized, after @ref windowInitTag.
    extern Tag screenPickerInitTag;

    /// @brief Window resize events are handled and the ScreenPicker texture is resized, after
    /// @ref windowPollTag and before @ref screenPickerClearTag.
    extern Tag screenPickerResizeTag;

    /// @brief Systems which draw to the screen picker should be tagged with this.
    extern Tag screenPickerDrawTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup screen-picker-plugin
    void screenPickerPlugin(Cubos& cubos);
} // namespace cubos::engine
