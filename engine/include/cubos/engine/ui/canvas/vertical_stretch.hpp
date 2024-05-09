/// @file
/// @brief Component @ref cubos::engine::VerticalStretch.
/// @ingroup canvas-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which makes a UI element fit vertically to its parent.
    /// @ingroup canvas-plugin
    struct CUBOS_ENGINE_API VerticalStretch
    {
        CUBOS_REFLECT;

        float topOffset = 0.0F;    ///< Margin to the top border of the parent
        float bottomOffset = 0.0F; ///< Margin to the bottom border of the parent
    };
} // namespace cubos::engine