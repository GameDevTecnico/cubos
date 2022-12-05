#ifndef CUBOS_CORE_IO_GLFW_WINDOW_HPP
#define CUBOS_CORE_IO_GLFW_WINDOW_HPP

#include <glm/glm.hpp>

#ifdef WITH_GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cubos/core/gl/ogl_render_device.hpp>
#endif

#include <cubos/core/io/window.hpp>

namespace cubos::core::io
{
    /// Wrapper around a window object, handles input events and creates the render device
    class GLFWWindow : public BaseWindow
    {
    public:
        GLFWWindow(const std::string& title, const glm::ivec2& size);
        virtual ~GLFWWindow() override;

        // Interface implementation.

        virtual void pollEvents() const override;
        virtual void swapBuffers() const override;
        virtual gl::RenderDevice& getRenderDevice() const override;
        virtual glm::ivec2 getSize() const override;
        virtual glm::ivec2 getFramebufferSize() const override;
        virtual bool shouldClose() const override;
        virtual double getTime() const override;
        virtual void setMouseState(MouseState state) override;
        virtual MouseState getMouseState() const override;
        virtual std::shared_ptr<Cursor> createCursor(Cursor::Standard standard) override;
        virtual void setCursor(std::shared_ptr<Cursor> cursor) override;
        virtual void setClipboard(const std::string& text) override;
        virtual const char* getClipboard() const override;

    private:
        std::shared_ptr<Cursor> cursor;

#ifdef WITH_GLFW
        GLFWwindow* handle;
        gl::OGLRenderDevice* renderDevice;
#endif
    };
} // namespace cubos::core::io

#endif // CUBOS_CORE_IO_GLFW_WINDOW_HPP
