/// @file
/// @brief Component @ref cubos::engine::GBuffer.
/// @ingroup render-g-buffer-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the GBuffer textures of a render target.
    /// @ingroup render-g-buffer-plugin
    struct CUBOS_ENGINE_API GBuffer
    {
        CUBOS_REFLECT;

        /// @brief Size of the GBuffer textures, in pixels.
        glm::uvec2 size = {0, 0};

        /// @brief Whether the GBuffer textures have already been cleared this frame.
        bool cleared = false;

        /// @brief Stores the position of the objects in the scene.
        core::gl::Texture2D position{nullptr};

        /// @brief Stores the normal of the objects in the scene.
        core::gl::Texture2D normal{nullptr};

        /// @brief Stores the albedo of the objects in the scene.
        core::gl::Texture2D albedo{nullptr};
    };
} // namespace cubos::engine
