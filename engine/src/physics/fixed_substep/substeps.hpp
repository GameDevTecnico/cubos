/// @file
/// @brief Resource @ref cubos::engine::Substeps.
/// @ingroup physics-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which holds the amount of substeps for the physics update.
    /// @ingroup physics-plugin
    struct Substeps
    {
        CUBOS_REFLECT;

        int value = 4;
    };
} // namespace cubos::engine
