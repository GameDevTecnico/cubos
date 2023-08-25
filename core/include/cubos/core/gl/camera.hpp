/// @file
/// @brief Class @ref cubos::core::gl::Camera.
/// @ingroup core-gl

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

namespace cubos::core::gl
{
    /// @brief Describes a camera.
    /// @todo This should be moved to the engine.
    /// @ingroup core-gl
    struct Camera
    {
        float fovY;     ///< Vertical field of view in degrees.
        float zNear;    ///< Near clipping plane.
        float zFar;     ///< Far clipping plane.
        glm::mat4 view; ///< Camera's view matrix.

        glm::ivec2 viewportPosition; ///< Bottom left corner of the viewport.
        glm::ivec2 viewportSize;     ///< Size of the viewport.
    };
} // namespace cubos::core::gl
