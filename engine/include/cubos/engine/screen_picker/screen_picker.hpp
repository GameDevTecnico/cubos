/// @file
/// @brief Resource @ref cubos::engine::ScreenPicker.
/// @ingroup screen-picker-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @brief Resource which provides a texture to store entity/gizmo ids, for selection with a mouse.
    ///
    /// @ingroup screen-picker-plugin
    class CUBOS_ENGINE_API ScreenPicker final
    {
    public:
        CUBOS_REFLECT;

        /// @brief Initializes ScreenPicker for a render device.
        /// @param currentRenderDevice Current Render device being used.
        /// @param size The size of the window in pixels.
        void init(cubos::core::gl::RenderDevice* currentRenderDevice, glm::ivec2 size);

        /// @brief Resizes the picking texture.
        /// @param size New size of the texture.
        void resizeTexture(glm::ivec2 size);

        /// @brief Clears the picking texture.
        void clearTexture();

        /// @brief Returns the framebuffer used to draw to the picking texture.
        /// @return Framebuffer used to draw to the picking texture.
        core::gl::Framebuffer framebuffer();

        /// @brief Returns the identifier associated to the pixel in the coords (x, y).
        /// @param x Coordinate of pixel in x axis
        /// @param y Coordinate of pixel in y axis
        /// @return Entity or gizmo identifier associated to the pixel in the coords (x, y).
        uint32_t at(int x, int y) const;

        /// @brief Returns the size of the picking texture in pixels.
        /// @return Size of the picking texture in pixels.
        glm::uvec2 size() const;

    private:
        cubos::core::gl::RenderDevice* mRenderDevice; ///< Active render device.
        core::gl::Framebuffer mIdFramebuffer;         ///< Framebuffer used to draw to the picking texture.
        core::gl::Texture2D mIdTexture; ///< Picking texture, stores entity/gizmo ids for each pixel on the screen.
        glm::ivec2 mTextureSize;        ///< Size of the picking texture in pixels.

        /// @brief Creates the picking texture with a specified size.
        /// @param size Size of the picking texture in pixels.
        void initIdTexture(glm::ivec2 size);
    };
} // namespace cubos::engine
