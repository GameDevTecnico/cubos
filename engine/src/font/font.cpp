#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/font/font.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Font)
{
    return cubos::core::reflection::Type::create("cubos::engine::Font");
}

namespace cubos::engine
{
    Font::Font(void* freetypeHandle, core::memory::Stream& stream)
    {
        if (freetypeHandle == nullptr)
        {
            CUBOS_ERROR("Couldn't load font: Invalid Freetype handle");
            return;
        }
        stream.seek(0, cubos::core::memory::SeekOrigin::End);
        size_t streamSize = stream.tell();
        mData.resize(streamSize);
        stream.seek(0, cubos::core::memory::SeekOrigin::Begin);
        if (stream.read(mData.data(), streamSize) < streamSize)
        {
            CUBOS_ERROR("Couldn't load font: Read less than stream length");
            return;
        }
        mHandle = msdfgen::loadFontData(static_cast<msdfgen::FreetypeHandle*>(freetypeHandle), mData.data(),
                                        static_cast<int>(streamSize));
        if (mHandle == nullptr)
        {
            CUBOS_ERROR("Couldn't load font: Failed to load data");
        }
    }

    Font::Font(Font&& other) noexcept
        : mHandle(other.mHandle)
        , mData(std::move(other.mData))
    {
        other.mHandle = nullptr;
    }

    Font::~Font()
    {
        if (mHandle != nullptr)
        {
            msdfgen::destroyFont(static_cast<msdfgen::FontHandle*>(mHandle));
        }
    }

    void* Font::handle() const
    {
        return mHandle;
    }

} // namespace cubos::engine
