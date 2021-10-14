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
        Window(); // TODO: pass settings here?
        ~Window();

        /// Polls window events, firing the events
        void pollEvents() const;

        /// Swaps the window buffers
        void swapBuffers() const;

        /// Returns the window render device
        gl::RenderDevice& getRenderDevice() const;
        
        /// Return the window framebuffer size in pixels
        glm::ivec2 getFramebufferSize() const;

        /// Should the window close?
        bool shouldClose() const;

    private:
        gl::RenderDevice* renderDevice; 
        void *handle;
    };
} // namespace cubos::io

#endif // CUBOS_IO_WINDOW_HPP
