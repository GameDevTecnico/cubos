#pragma once

#include <deque>

#include <glm/glm.hpp>

#ifdef WITH_GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../gl/ogl_render_device.hpp"
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
        gl::RenderDevice& renderDevice() const override;
        glm::uvec2 size() const override;
        glm::uvec2 framebufferSize() const override;
        float contentScale() const override;
        bool shouldClose() const override;
        double time() const override;
        void mouseState(MouseState state) override;
        MouseState mouseState() const override;
        glm::ivec2 getMousePosition() override;
        void setMousePosition(glm::ivec2 pos) override;
        std::shared_ptr<Cursor> createCursor(Cursor::Standard standard) override;
        void cursor(std::shared_ptr<Cursor> cursor) override;
        void clipboard(const std::string& text) override;
        const char* clipboard() const override;
        Modifiers modifiers() const override;
        bool pressed(Key key, Modifiers modifiers = Modifiers::None) const override;
        bool gamepadState(int gamepad, GamepadState& state) const override;

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
