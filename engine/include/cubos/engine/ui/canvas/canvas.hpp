/// @file
/// @brief Component @ref cubos::engine::UICanvas.
/// @ingroup ui-canvas-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which represents a target for UI rendering.
    /// @ingroup ui-canvas-plugin
    struct CUBOS_ENGINE_API UICanvas
    {
        CUBOS_REFLECT;
        glm::vec2 referenceSize = {1920.0F,
                                   1080.0F}; ///< Size to be used as reference for the elements below this canvas.
        glm::vec2 virtualSize;               ///< Internal size of the canvas, with scalling applied.
        glm::mat4 mat;                       ///< View matrix provided of the canvas.
    };
} // namespace cubos::engine