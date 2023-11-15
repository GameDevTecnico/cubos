/// @file
/// @brief Resource @ref cubos::engine::Substeps.
/// @ingroup physics-plugin

#pragma once

namespace cubos::engine
{
    /// @brief Resource which holds the amount of substeps for the physics update.
    /// @ingroup physics-plugin
    struct Substeps
    {
        int value = 1;
    };
} // namespace cubos::engine
