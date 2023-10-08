/// @dir
/// @brief @ref transform-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup transform-plugin

#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/transform/transform.hpp>

namespace cubos::engine
{
    /// @defgroup transform-plugin Transform
    /// @ingroup engine
    /// @brief Adds transform components.
    ///
    /// ## Components
    /// - @ref Transform - Component which stores the transformation matrix of an entity.

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup transform-plugin
    void transformPlugin(Cubos& cubos);
} // namespace cubos::engine
