/// @file
/// @brief Component @ref cubos::engine::HorizontalStretch.
/// @ingroup canvas-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which makes a UI element fit horizontally to its parent.
    /// @ingroup canvas-plugin
    struct CUBOS_ENGINE_API HorizontalStretch
    {
        CUBOS_REFLECT;

        float leftOffset = 0.0F;  ///< Margin to the left border of the parent
        float rightOffset = 0.0F; ///< Margin to the right border of the parent
    };
} // namespace cubos::engine