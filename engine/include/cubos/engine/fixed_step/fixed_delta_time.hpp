/// @file
/// @brief Resource @ref cubos::engine::FixedDeltaTime.
/// @ingroup fixed-step-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Resource which holds the value of the fixed delta for the fixedStep plugin.
    /// @ingroup fixed-step-plugin
    struct FixedDeltaTime
    {
        CUBOS_REFLECT;

        float value = 1.0F / 100.0F;
    };
} // namespace cubos::engine
