/// @file
/// @brief Component @ref cubos::engine::UIKeepPixelSize.
/// @ingroup ui-canvas-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which makes a UI canvas scale to keep its elements at the same pixel size.
    /// @ingroup ui-canvas-plugin
    struct CUBOS_ENGINE_API UIKeepPixelSize
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine