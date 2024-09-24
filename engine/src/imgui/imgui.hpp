/// @file
/// @brief Functions for initializing and using ImGui.
/// @ingroup imgui-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>

namespace cubos::engine
{
    /// @brief Initializes ImGui for use with the given window.
    /// @note Should only be called once and no ImGui calls should be made before this is called.
    /// @param window The window to use.
    /// @param dpiScale Size factor by which to scale ImGui elements.
    /// @ingroup imgui-plugin
    void imguiInitialize(const core::io::Window& window, float dpiScale);

    /// @brief Shuts down ImGui.
    /// @note Should only be called once, after @ref initialize(), and no ImGui calls should be
    /// made after this is called.
    /// @ingroup imgui-plugin
    void imguiTerminate();

    /// @brief Begins a new ImGui frame. ImGui calls should be made between this and @ref
    /// endFrame().
    /// @ingroup imgui-plugin
    void imguiBeginFrame();

    /// @brief Ends the current ImGui frame, and renders the ImGui draw data to the @p target
    /// framebuffer, or the default framebuffer if @p target is null.
    /// @param target Framebuffer to render to.
    /// @ingroup imgui-plugin
    void imguiEndFrame(const core::gl::Framebuffer& target = nullptr);

    /// @brief Passes a window event to ImGui.
    /// @param event Event to pass.
    /// @return True if the event was handled by ImGui, false otherwise.
    /// @ingroup imgui-plugin
    bool imguiHandleEvent(const core::io::WindowEvent& event);
} // namespace cubos::engine
