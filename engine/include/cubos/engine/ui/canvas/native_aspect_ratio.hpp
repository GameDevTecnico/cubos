/// @file
/// @brief Component @ref cubos::engine::UINativeAspectRatio.
/// @ingroup ui-canvas-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which makes a UI element keep the aspect ratio of its source.
    /// @ingroup ui-canvas-plugin
    struct CUBOS_ENGINE_API UINativeAspectRatio
    {
        CUBOS_REFLECT;
        float ratio = 0.0F; ///< Aspect ratio of the source.
    };
} // namespace cubos::engine