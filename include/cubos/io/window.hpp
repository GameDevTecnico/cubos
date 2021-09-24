#ifndef CUBOS_IO_WINDOW_HPP
#define CUBOS_IO_WINDOW_HPP

#include <glm/glm.hpp>

namespace cubos::gl
{
    class RenderDevice;
}

namespace cubos::io
{
    // TODO: Format documentation (which style?)
    // Wrapper around a window object, handles input events and creates the render device
    class Window
    {
    public:
        Window(); // TODO: pass settings here?
        ~Window();


        void pollEvents() const;
        void swapBuffers() const;

        const gl::RenderDevice& getRenderDevice() const;
        glm::ivec2 getFramebufferSize() const;
        bool shouldClose() const;

    private:
        gl::RenderDevice* renderDevice; 
        void *handle;
    };
} // namespace cubos::io

#endif // CUBOS_IO_WINDOW_HPP
