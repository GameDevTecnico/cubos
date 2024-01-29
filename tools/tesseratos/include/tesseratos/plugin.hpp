/// @dir
/// @brief @ref tesseratos module.

/// @file
/// @brief Plugin entry point.
/// @ingroup tesseratos

#pragma once

#include <cubos/engine/prelude.hpp>

/// @brief @ref tesseratos module.
namespace tesseratos
{
    /// @defgroup tesseratos Tesseratos
    /// @brief @b TESSERATOS. library.
    ///
    /// The tesseratos library provides tools useful for debugging and developing games using
    /// @b CUBOS. It is built on top of the @ref engine "engine library". Each sub-module
    /// corresponds to a tool or a set of tools.

    /// @brief Tesseratos entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup tesseratos
    void plugin(cubos::engine::Cubos& cubos);
} // namespace tesseratos
