/// @dir
/// @brief @ref metrics-panel-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup metrics-panel-tool-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup metrics-panel-tool-plugin Metrics Panel
    /// @ingroup tool-plugins
    /// @brief Shows some useful performance metrics through a ImGui window.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup metrics-panel-tool-plugin
    void metricsPanelPlugin(Cubos& cubos);
} // namespace cubos::engine
