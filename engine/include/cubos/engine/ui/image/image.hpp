/// @file
/// @brief Component @ref cubos::engine::UIImage.
/// @ingroup ui-image-plugin

#pragma once

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/image/image.hpp>

namespace cubos::engine
{
    /// @brief Component used to draw images on the UI.
    ///
    /// @ingroup ui-image-plugin
    struct CUBOS_ENGINE_API UIImage
    {
        CUBOS_REFLECT;

        /// @brief Handle to the image asset to be rendered.
        ///
        /// When null, no image will be rendered.
        Asset<Image> asset{};

        cubos::core::gl::Texture2D texture = nullptr; ///< Handle to the generated 2D texture.
    };
} // namespace cubos::engine
