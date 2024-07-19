/// @file
/// @brief Component @ref cubos::engine::UIExpand.
/// @ingroup ui-canvas-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which makes a UI canvas scale to maintain proportions, choosing the dimension which reduces the
    /// element sizes on screen.
    /// @ingroup ui-canvas-plugin
    struct CUBOS_ENGINE_API UIExpand
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine