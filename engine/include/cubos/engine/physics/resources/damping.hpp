/// @file
/// @brief Resource @ref cubos::engine::Damping.
/// @ingroup physics-plugin

#pragma once

namespace cubos::engine
{
    /// @brief Resource which holds the damping value for integration.
    /// @ingroup physics-plugin
    struct Damping
    {
        float value = 0.999F;
    };
} // namespace cubos::engine
