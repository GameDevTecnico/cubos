/// @file
/// @brief Functions for initializing and using ImGui.
/// @ingroup core-ui

#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>

namespace cubos::core::ui
{
    /// @brief Initializes ImGui for use with the given window.
    /// @note Should only be called once and no ImGui calls should be made before this is called.
    /// @param window The window to use.
    /// @ingroup core-ui
    void initialize(io::Window window);

    /// @brief Shuts down ImGui.
    /// @note Should only be called once, after @ref initialize(), and no ImGui calls should be
    /// made after this is called.
    /// @ingroup core-ui
    void terminate();

    /// @brief Begins a new ImGui frame. ImGui calls should be made between this and @ref
    /// endFrame().
    /// @ingroup core-ui
    void beginFrame();

    /// @brief Ends the current ImGui frame, and renders the ImGui draw data to the @p target
    /// framebuffer, or the default framebuffer if @p target is null.
    /// @param target Framebuffer to render to.
    /// @ingroup core-ui
    void endFrame(const gl::Framebuffer& target = nullptr);

    /// @brief Passes a window event to ImGui.
    /// @param event Event to pass.
    /// @return True if the event was handled by ImGui, false otherwise.
    /// @ingroup core-ui
    bool handleEvent(const io::WindowEvent& event);
} // namespace cubos::core::ui
