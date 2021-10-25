#ifndef CUBOS_IO_WINDOW_HPP
#define CUBOS_IO_WINDOW_HPP

#include <glm/glm.hpp>

namespace cubos::gl
{
    class RenderDevice;
}

namespace cubos::io
{
    /// Wrapper around a window object, handles input events and creates the render device
    class Window
    {
    public:
        /// Polls window events, firing the events
        virtual void pollEvents() const = 0;

        /// Swaps the window buffers
        virtual void swapBuffers() const = 0;

        /// Returns the window render device
        gl::RenderDevice& getRenderDevice() const;

        /// Return the window framebuffer size in pixels
        virtual glm::ivec2 getFramebufferSize() const = 0;

        /// Should the window close?
        virtual bool shouldClose() const = 0;

    protected:
        gl::RenderDevice* renderDevice;
        void initRenderDevice();
        void destroyRenderDevice();
    };
} // namespace cubos::io

#endif // CUBOS_IO_WINDOW_HPP
