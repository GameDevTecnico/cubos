/// @dir
/// @brief @ref tesseratos-layout-bridge-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-layout-bridge-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-layout-bridge-plugin Layout Bridge
    /// @ingroup tesseratos
    /// @brief Adds a bridge to save and load docking layouts.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-layout-bridge-plugin
    void layoutBridgePlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
