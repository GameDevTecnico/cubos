#ifndef CUBOS_CORE_GL_CAMERA_DATA_HPP
#define CUBOS_CORE_GL_CAMERA_DATA_HPP

#include <cubos/core/gl/render_device.hpp>
#include <glm/glm.hpp>

namespace cubos::core::gl
{
    struct CameraData
    {
        glm::mat4 viewMatrix; ///< The Camera's view matrix.

        glm::mat4 perspectiveMatrix; ///< The camera's perspective matrix.

        gl::Framebuffer target; ///< The target framebuffer which the camera should be used to draw into.
    };
} // namespace cubos::core::gl

#endif // CUBOS_CORE_GL_CAMERA_DATA_HPP
