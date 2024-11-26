/// @file
/// @brief Component @ref cubos::engine::UIText.
/// @ingroup ui-text-plugin

#pragma once

#include <string>

#include <glm/vec4.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/font/atlas/atlas.hpp>
#include <cubos/engine/font/font.hpp>

namespace cubos::engine
{
    /// @brief Component used to draw text on the UI.
    ///
    /// @ingroup ui-text-plugin
    struct CUBOS_ENGINE_API UIText
    {
        CUBOS_REFLECT;

        /// @brief The text of the element.
        std::string text;

        /// @brief The color of the text.
        glm::vec4 color{1.0F};

        /// @brief The font size to draw the characters.
        float fontSize{24.0F};

        /// @brief The font atlas to be used.
        Asset<FontAtlas> fontAtlas{AnyAsset("ab32bedd-8ea5-49f0-bca6-0b319feb1207")};
    };
} // namespace cubos::engine
