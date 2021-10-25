#ifndef CUBOS_IO_GLFW_WINDOW_IMPL_HPP
#define CUBOS_IO_GLFW_WINDOW_IMPL_HPP

#include <glm/glm.hpp>
#define GLFW_INCLUDE_NONE
#include <glfw/glfw3.h>

#include <cubos/gl/ogl_render_device.hpp>
#include <cubos/io/window.hpp>

namespace cubos::io
{
    /// Wrapper around a window object, handles input events and creates the render device
    class GLFWWindow : public cubos::io::Window
    {
    public:
        GLFWWindow(); // TODO: pass settings here?
        virtual ~GLFWWindow() override;

        /// Polls window events, firing the events
        virtual void pollEvents() const override;

        /// Swaps the window buffers
        virtual void swapBuffers() const override;

        /// Returns the window render device
        virtual gl::RenderDevice& getRenderDevice() const override;

        /// Return the window framebuffer size in pixels
        virtual glm::ivec2 getFramebufferSize() const override;

        /// Should the window close?
        virtual bool shouldClose() const override;

    private:
        GLFWwindow* handle;
        gl::OGLRenderDevice* renderDevice;
    };
} // namespace cubos::io

#endif // CUBOS_IO_GLFW_WINDOW_IMPL_HPP
