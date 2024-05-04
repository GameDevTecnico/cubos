/// @file
/// @brief Resource @ref cubos::engine::FixedDeltaTime.
/// @ingroup fixed-step-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which holds the value of the fixed delta for the fixedStep plugin.
    /// @ingroup fixed-step-plugin
    struct CUBOS_ENGINE_API FixedDeltaTime
    {
        CUBOS_REFLECT;

        float value = 1.0F / 30.0F;
    };
} // namespace cubos::engine
