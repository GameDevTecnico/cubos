/// @file
/// @brief Resource @ref cubos::engine::Substeps.
/// @ingroup physics-plugin

#pragma once

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which holds the amount of substeps for the physics update.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API Substeps
    {
        CUBOS_REFLECT;

        int count = 0;
        int value = 4;
    };
} // namespace cubos::engine
