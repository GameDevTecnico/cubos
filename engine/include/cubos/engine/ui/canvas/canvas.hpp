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
        float width = 1920.0F;  ///< Width of the canvas in global coordinates
        float height = 1080.0F; ///< Height of the canvas in global coordinates

        glm::mat4 mat; ///< View matrix provided of the canvas.
    };
} // namespace cubos::engine