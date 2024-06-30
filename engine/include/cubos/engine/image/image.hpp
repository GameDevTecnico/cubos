/// @file
/// @brief Class @ref cubos::engine::Image.
/// @ingroup image-plugin

#pragma once

#include <unordered_map>

#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>

namespace cubos::engine
{
    /// @brief Asset containing raw Image data.
    ///
    /// @ingroup image-plugin
    struct CUBOS_ENGINE_API Image
    {
        CUBOS_REFLECT;
        unsigned char* data; ///< Raw image data.
        size_t width;        ///< Image width in pixels.
        size_t height;       ///< Image height in pixels.

        explicit Image(core::memory::Stream& stream);
        Image(Image&& other) noexcept;
        ~Image();
    };
} // namespace cubos::engine
