/// @file
/// @brief Resource @ref cubos::engine::ImGuiContextHolder.
/// @ingroup imgui-plugin

#pragma once

#include <imgui.h>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores a pointer to the ImGui context created by the ImGui plugin.
    ///
    /// This resource is inserted by the @ref imgui-plugin during @ref imguiInitTag.
    ///
    /// When linking dynamically the engine library from an application or another library, although this plugin
    /// initializes the context, it might not be set automatically on the client side - particularly, this happens on
    /// Windows, as global variables are not shared between DLLs and applications.
    ///
    /// So, if you're using ImGui in your application or library, to ensure dynamic linking capability in all platforms,
    /// you should call `ImGui::SetCurrentContext(ImGuiContextHolder::context)` before using any ImGui functions.
    ///
    /// This resource is not named ImGuiContext because ImGui already has a type with that name.
    ///
    /// @ingroup imgui-plugin
    struct CUBOS_ENGINE_API ImGuiContextHolder
    {
        CUBOS_REFLECT;

        /// @brief Pointer to the ImGui context.
        ImGuiContext* context;
    };
} // namespace cubos::engine
