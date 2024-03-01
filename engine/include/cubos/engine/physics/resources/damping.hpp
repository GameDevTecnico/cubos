/// @file
/// @brief Resource @ref cubos::engine::Damping.
/// @ingroup physics-plugin

#pragma once

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which holds the damping value for integration.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API Damping
    {
        CUBOS_REFLECT;

        float value = 0.99F;
    };
} // namespace cubos::engine
