/// @file
/// @brief Resource @ref cubos::engine::FixedAccumulatedTime.
/// @ingroup fixed-step-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which holds the accumulated time for the fixedStep plugin.
    /// @ingroup fixed-step-plugin
    struct CUBOS_ENGINE_API FixedAccumulatedTime
    {
        CUBOS_REFLECT;

        float value = 0.0F;
    };
} // namespace cubos::engine
