/// @dir
/// @brief @ref render-split-screen-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup render-split-screen-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup render-split-screen-plugin Splitscreen
    /// @ingroup render-plugins
    /// @brief Adjusts the viewport in @ref DrawsTo relations to achieve a splitscreen layout.
    ///
    /// ## Dependencies
    /// - @ref render-target-plugin
    /// - @ref render-camera-plugin
    /// - @ref transform-plugin

    /// @brief Tags the system which adjusts the viewports.
    /// @ingroup render-split-screen-plugin
    CUBOS_ENGINE_API extern Tag splitScreenTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup render-split-screen-plugin
    CUBOS_ENGINE_API void splitScreenPlugin(Cubos& cubos);
} // namespace cubos::engine
