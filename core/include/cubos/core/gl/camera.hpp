#pragma once

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

namespace cubos::core::gl
{
    /// Holds data describing a camera.
    struct Camera
    {
        float fovY;     ///< The vertical field of view in degrees.
        float zNear;    ///< The near clipping plane.
        float zFar;     ///< The far clipping plane.
        glm::mat4 view; ///< The camera's view matrix.

        glm::ivec2 viewportPosition; ///< Bottom left corner of the viewport.
        glm::ivec2 viewportSize;     ///< Size of the viewport.
    };
} // namespace cubos::core::gl
