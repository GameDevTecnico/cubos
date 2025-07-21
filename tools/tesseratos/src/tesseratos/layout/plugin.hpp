/// @dir
/// @brief @ref tesseratos-layout-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-layout-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-layout-plugin Menu bar
    /// @ingroup tesseratos
    /// @brief Manages the docking layouts for tesseratos.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-layout-plugin
    void layoutPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
