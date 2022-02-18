#ifndef CUBOS_CORE_GL_IMGUI_IMPL_HPP
#define CUBOS_CORE_GL_IMGUI_IMPL_HPP

#include <cubos/core/io/window.hpp>

#include <imgui.h>

IMGUI_IMPL_API bool ImGui_ImplCubos_Init(cubos::io::Window& window);
IMGUI_IMPL_API void ImGui_ImplCubos_Shutdown();
IMGUI_IMPL_API void ImGui_ImplCubos_NewFrame();
IMGUI_IMPL_API void ImGui_ImplCubos_RenderDrawData(ImDrawData* drawData);

#endif // CUBOS_CORE_GL_IMGUI_IMPL_HPP
