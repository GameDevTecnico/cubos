/// @file
/// @brief Component @ref cubos::engine::UIColorRect.
/// @ingroup color-rect-plugin

#pragma once

#include <glm/vec4.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component that draws a UI element as a solid color.
    /// @ingroup color-rect-plugin
    struct CUBOS_ENGINE_API UIColorRect
    {
        CUBOS_REFLECT;
        glm::vec4 color = {0, 0, 0, 1}; ///< Color to draw element as.
    };
} // namespace cubos::engine