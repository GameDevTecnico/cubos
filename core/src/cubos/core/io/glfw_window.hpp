#pragma once

#include <deque>

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
        ~GLFWWindow() override;

        // Interface implementation.

        void pollEvents() override;
        void swapBuffers() override;
        gl::RenderDevice& getRenderDevice() const override;
        glm::ivec2 getSize() const override;
        glm::ivec2 getFramebufferSize() const override;
        bool shouldClose() const override;
        double getTime() const override;
        void setMouseState(MouseState state) override;
        MouseState getMouseState() const override;
        std::shared_ptr<Cursor> createCursor(Cursor::Standard standard) override;
        void setCursor(std::shared_ptr<Cursor> cursor) override;
        void setClipboard(const std::string& text) override;
        const char* getClipboard() const override;
        bool keyPressed(Key key) const override;
        bool keyPressed(Key key, Modifiers modifiers) const override;

        Modifiers modifiers() const;
        void modifiers(Modifiers modifiers);

    private:
        std::shared_ptr<Cursor> mCursor;
        Modifiers mModifiers;

#ifdef WITH_GLFW
        GLFWwindow* mHandle;
        gl::OGLRenderDevice* mRenderDevice;
#endif
    };
} // namespace cubos::core::io
