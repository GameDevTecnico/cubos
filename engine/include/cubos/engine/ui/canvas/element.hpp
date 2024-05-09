/// @file
/// @brief Component @ref cubos::engine::UIElement.
/// @ingroup ui-canvas-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

using namespace cubos::core::gl;

namespace cubos::engine
{
    /// @brief Component which defines the space an element of the UI occupies as a rect.
    /// @ingroup ui-canvas-plugin
    struct CUBOS_ENGINE_API UIElement
    {
        CUBOS_REFLECT;

        glm::vec2 offset = {0.0F, 0.0F}; ///< Offset of the pivot.
        glm::vec2 size = {0.0F, 0.0F};   ///< Size of the element.
        glm::vec2 pivot = {0.5F, 0.5F};  ///< Relative position of the pivot to the element itself.
        glm::vec2 anchor = {0.5F, 0.5F}; ///< Relative position of the pivot to the parent element.
        int layer = 0;                   ///< Rendering order layer.

        glm::vec2 position = {0.0F, 0.0F}; ///< Global position of the pivot.
        glm::mat4 vp;                      ///< View matrix provided by the canvas.

        int hierarchyDepth = 0; ///< Depth in hierarchy. Used for depth sorting.

        /// @brief Gets global position of the bottom left corner of the element.
        /// @return Position value in global coordinates.
        glm::vec2 bottomLeftCorner() const;

        /// @brief Gets global position of the top right corner of the element.
        /// @return Position value in global coordinates.
        glm::vec2 topRightCorner() const;
    };
} // namespace cubos::engine