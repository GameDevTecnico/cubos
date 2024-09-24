/// @dir
/// @brief @ref ecs-statistics-tool-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup ecs-statistics-tool-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup ecs-statistics-tool-plugin ECS Statistics
    /// @ingroup tool-plugins
    /// @brief Shows tons of statistics and information about the internal state of the ECS.

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup ecs-statistics-tool-plugin
    void ecsStatisticsPlugin(Cubos& cubos);
} // namespace cubos::engine
