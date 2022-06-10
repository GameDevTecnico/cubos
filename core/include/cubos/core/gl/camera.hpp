#ifndef CUBOS_CORE_GL_CAMERA_HPP
#define CUBOS_CORE_GL_CAMERA_HPP

#include <cubos/core/gl/render_device.hpp>

#include <glm/glm.hpp>

namespace cubos::core::gl
{
    /// Holds data describing a camera.
    struct Camera
    {
        float fovY;     ///< The vertical field of view in degrees.
        float zNear;    ///< The near clipping plane.
        float zFar;     ///< The far clipping plane.
        glm::mat4 view; ///< The camera's view matrix.
    };
} // namespace cubos::core::gl

#endif // CUBOS_CORE_GL_CAMERA_HPP
