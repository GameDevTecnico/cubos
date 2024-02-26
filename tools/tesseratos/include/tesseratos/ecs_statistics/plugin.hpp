/// @dir
/// @brief @ref tesseratos-ecs-statistics-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-ecs-statistics-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

namespace tesseratos
{
    /// @defgroup tesseratos-ecs-statistics-plugin ECS Statistics
    /// @ingroup tesseratos
    /// @brief Shows tons of statistics and information about the internal state of the ECS.
    ///
    /// ## Dependencies
    /// - @ref imgui-plugin
    /// - @ref tesseratos-toolbox-plugin
    /// - @ref tesseratos-entity-selector-plugin

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos-ecs-statistics-plugin
    void ecsStatisticsPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
