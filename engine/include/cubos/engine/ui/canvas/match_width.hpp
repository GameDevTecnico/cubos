/// @file
/// @brief Component @ref cubos::engine::UIMatchWidth.
/// @ingroup ui-canvas-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which makes a UI canvas scale to maintain horizontal proportions.
    /// @ingroup ui-canvas-plugin
    struct CUBOS_ENGINE_API UIMatchWidth
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine