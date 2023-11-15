/// @file
/// @brief Resource @ref cubos::engine::FixedDeltaTime.
/// @ingroup physics-plugin

#pragma once

namespace cubos::engine
{
    /// @brief Resource which holds the value of the fixed delta for the physics update.
    /// @ingroup physics-plugin
    struct FixedDeltaTime
    {
        float value = 1.0F / 100.0F;
    };
} // namespace cubos::engine
