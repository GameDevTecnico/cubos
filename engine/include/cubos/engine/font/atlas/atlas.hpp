/// @file
/// @brief Struct @ref cubos::engine::FontAtlas.
/// @ingroup font-plugin

#pragma once

#include <cstdint>
#include <optional>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/font/atlas/glyph.hpp>

namespace cubos::engine
{
    /// @brief Class that holds all the necessary data about a font atlas. This font atlas represents the texure
    /// created from all the different glyphs in a font, that will be then used for drawing the text.
    ///
    /// @ingroup font-plugin
    class CUBOS_ENGINE_API FontAtlas
    {
    public:
        CUBOS_REFLECT;

        FontAtlas(void* font, double minimumScale, double pixelRange, double miterLimit,
                  cubos::core::gl::RenderDevice& renderDevice);

        std::optional<FontGlyph> requestGlyph(uint32_t unicode) const;

        /// @brief Returns the GPU texture created with this font atlas.
        cubos::core::gl::Texture2D texture() const;

    private:
        cubos::core::gl::Texture2D mTexture{nullptr};

        std::unordered_map<uint32_t, FontGlyph> mGlyphs;
    };

} // namespace cubos::engine
