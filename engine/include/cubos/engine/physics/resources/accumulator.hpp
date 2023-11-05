/// @file
/// @brief Resource @ref cubos::engine::Accumulator.
/// @ingroup physics-plugin

#pragma once

namespace cubos::engine
{
    /// @brief Resource which holds the progress to the next integration.
    /// @ingroup physics-plugin
    struct Accumulator
    {
        float value = 0.0F;
    };
} // namespace cubos::engine
