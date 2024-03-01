/// @dir
/// @brief @ref render-picker-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-picker-plugin

#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-picker-plugin RenderPicker
    /// @ingroup render-plugins
    /// @brief Adds and manages @ref RenderPicker components.
    ///
    /// ## Dependencies
    /// - @ref window-plugin
    /// - @ref render-target-plugin

    /// @brief Recreates the RenderPicker if necessary - for example, due to a render target resize.
    CUBOS_ENGINE_API extern Tag createRenderPickerTag;

    /// @brief Systems which draw to RenderPicker textures should be tagged with this.
    CUBOS_ENGINE_API extern Tag drawToRenderPickerTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class.
    /// @ingroup render-picker-plugin
    CUBOS_ENGINE_API void renderPickerPlugin(Cubos& cubos);
} // namespace cubos::engine
