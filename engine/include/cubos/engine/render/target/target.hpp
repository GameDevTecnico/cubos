/// @file
/// @brief Component @ref cubos::engine::RenderTarget.
/// @ingroup render-target-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which represents a render target.
    ///
    /// If the framebuffer is set to nullptr, the target will be assumed to be the window.
    /// In that case, the size will be automatically updated to the window's size.
    ///
    /// @ingroup render-target-plugin
    struct RenderTarget
    {
        CUBOS_REFLECT;

        /// @brief Size of the target texture, in pixels.
        glm::uvec2 size = {0, 0};

        /// @brief Framebuffer to render to.
        core::gl::Framebuffer framebuffer{nullptr};
    };
} // namespace cubos::engine
