/// @file
/// @brief Component @ref cubos::engine::RenderPicker.
/// @ingroup render-picker-plugin

#pragma once

#include <cstdint>

#include <glm/vec2.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which provides a texture to store entity/gizmo ids, for selection with a mouse.
    /// @ingroup render-picker-plugin
    struct CUBOS_ENGINE_API RenderPicker
    {
        CUBOS_REFLECT;

        /// @brief Size of the RenderPicker textures, in pixels.
        glm::uvec2 size = {0, 0};

        /// @brief Whether the RenderPicker front texture has already been cleared this frame.
        bool cleared = false;

        /// @brief Picking back texture, stores entity/gizmo ids for each pixel on the screen. Access to the
        /// picker results is made through this texture, to ensure the CPU doesn't wait for the current frame to finish.
        core::gl::Texture2D backTexture{nullptr};

        /// @brief Picking front texture, stores entity/gizmo ids for each pixel on the screen. This is the texture
        /// used by the renderer to write ids. It is automatically swapped with the back texture by the
        /// @ref renderPickerPlugin.
        core::gl::Texture2D frontTexture{nullptr};

        /// @brief Pixel pack buffer used to access the data in the @ref backTexture.
        core::gl::PixelPackBuffer pixelBuffer{nullptr};

        /// @brief Reads the identifier at the given pixel coordinates.
        /// @note Must be called on the render device thread.
        /// @param x X coordinate.
        /// @param y Y coordinate.
        /// @return Identifier.
        uint32_t read(unsigned int x, unsigned int y) const;
    };
} // namespace cubos::engine
