/// @dir
/// @brief @ref tesseratos-metrics-panel-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-metrics-panel-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-metrics-panel-plugin Metrics
    /// @ingroup tesseratos
    /// @brief Shows some useful performance metrics through a ImGui window.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin
    /// - @ref tesseratos-toolbox-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-metrics-panel-plugin
    void metricsPanelPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
