/// @file
/// @brief Class @ref tesseratos::GameWindow.
/// @ingroup tesseratos-game-window-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/io/window.hpp>

#include "render_device.hpp"

namespace tesseratos
{
    /// @brief Implements the window abstract class for a ImGui window.
    /// @ingroup tesseratos-game-window-plugin
    class GameWindow : public cubos::core::io::BaseWindow
    {
    public:
        /// @brief Constructor.
        /// @param window Real window.
        GameWindow(cubos::core::io::Window window);

        /// @brief Resets the window to its initial state.
        void reset();

        /// @brief Handles an event from the real window.
        /// @param event Event to handle.
        void handleEvent(cubos::core::io::WindowEvent event);

        /// @brief Updates the minimum coordinates of the window, in the real window space.
        /// @param coords New minimum coordinates.
        void setMinCoords(glm::uvec2 coords);

        /// @brief Sets whether the window is focused.
        /// @param focused Whether the window is focused.
        void setFocused(bool focused);

        /// @brief Sets the new window size.
        /// @param size Window size.
        void resize(glm::uvec2 size);

        /// @brief Gets the texture with the rendered output.
        /// @return Texture with the rendered output.
        cubos::core::gl::Texture2D output() const;

        /// @brief Checks whether the window is focused.
        /// @return Whether the window is focused.
        bool focused() const;

        // Interface implementation.

        void pollEvents() override;
        void swapBuffers() override;
        cubos::core::gl::RenderDevice& renderDevice() const override;
        glm::uvec2 size() const override;
        glm::uvec2 framebufferSize() const override;
        float contentScale() const override;
        bool shouldClose() const override;
        double time() const override;
        void mouseState(cubos::core::io::MouseState state) override;
        cubos::core::io::MouseState mouseState() const override;
        glm::ivec2 getMousePosition() override;
        void setMousePosition(glm::ivec2 pos) override;
        std::shared_ptr<cubos::core::io::Cursor> createCursor(cubos::core::io::Cursor::Standard standard) override;
        void cursor(std::shared_ptr<cubos::core::io::Cursor> cursor) override;
        void clipboard(const std::string& text) override;
        const char* clipboard() const override;
        cubos::core::io::Modifiers modifiers() const override;
        bool pressed(cubos::core::io::Key key,
                     cubos::core::io::Modifiers modifiers = cubos::core::io::Modifiers::None) const override;
        bool gamepadState(int gamepad, cubos::core::io::GamepadState& state) const override;

    private:
        cubos::core::io::Window mWindow;
        mutable GameRenderDevice mRenderDevice;
        glm::uvec2 mMinCoords{0, 0};
        glm::uvec2 mSize{1, 1};
        double mStartTime{0.0};
        bool mFocused{false};
        cubos::core::io::MouseState mMouseState{cubos::core::io::MouseState::Default};
        std::shared_ptr<cubos::core::io::Cursor> mCursor{nullptr};
    };
} // namespace tesseratos
