#ifndef CUBOS_CORE_UI_IMGUI_HPP
#define CUBOS_CORE_UI_IMGUI_HPP

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>

namespace cubos::core::ui
{
    /// Initializes ImGui for use with the given window.
    /// Should only be called once and no ImGui calls should be made before this is called.
    /// @param window The window to use.
    void initialize(io::Window window);

    /// Shuts down ImGui.
    /// Should only be called once, after @ref initialize, and no ImGui calls should be made after this is called.
    void terminate();

    /// Begins a new ImGui frame.
    void beginFrame();

    /// Ends the current ImGui frame, and renders the ImGui draw data to the window.
    void endFrame(const gl::Framebuffer& target = nullptr);

    /// Passes a window event to ImGui.
    /// @param event The event to pass.
    /// @return True if the event was consumed by ImGui, false otherwise.
    bool handleEvent(const io::WindowEvent& event);
} // namespace cubos::core::ui

#endif // CUBOS_CORE_UI_IMGUI_HPP
