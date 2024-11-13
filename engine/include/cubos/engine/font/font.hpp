/// @file
/// @brief Struct @ref cubos::engine::Font.
/// @ingroup font-plugin

#pragma once

#include <vector>

#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Asset containing raw font data containing the glyphs and their shape. This is used to create @ref
    /// FontAtlas that can be used for rendering text.
    ///
    /// @ingroup font-plugin
    class CUBOS_ENGINE_API Font
    {
    public:
        CUBOS_REFLECT;

        /// @brief Creates a new Font asset from the data in the stream.
        Font(void* freetypeHandle, core::memory::Stream& stream);

        /// @brief Move constructor that moves the font handle to another Font asset.
        Font(Font&& other) noexcept;

        /// @brief Destroys the associated handle to the font.
        ~Font();

        /// @brief Returns the handle to this loaded font.
        void* handle() const;

    private:
        void* mHandle{nullptr};

        std::vector<uint8_t> mData;
    };
} // namespace cubos::engine
