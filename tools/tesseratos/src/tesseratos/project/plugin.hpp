/// @dir
/// @brief @ref tesseratos-project-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos-project-plugin

#pragma once

#include <cubos/engine/prelude.hpp>

#include "manager.hpp"

namespace tesseratos
{
    /// @defgroup tesseratos-project-plugin Project
    /// @ingroup tesseratos
    /// @brief Adds a resource used to manage the currently loaded project.

    /// @brief Tool state.
    struct Project
    {
        CUBOS_REFLECT;

        bool isOpen{false};
    };

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class
    /// @ingroup tesseratos-project-plugin
    void projectPlugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
