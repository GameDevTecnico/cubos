#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/audio/audio.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Audio)
{
    return core::reflection::Type::create("cubos::engine::Audio");
}

cubos::engine::Audio::Audio(std::shared_ptr<cubos::core::al::MiniaudioContext> context, core::memory::Stream& stream)
{
    stream.seek(0, cubos::core::memory::SeekOrigin::End);
    size_t streamSize = stream.tell();
    void* contents = operator new(sizeof(char) * streamSize);
    stream.seek(0, cubos::core::memory::SeekOrigin::Begin);
    if (stream.read((char*)contents, streamSize) < streamSize)
    {
        CUBOS_ERROR("Couldn't load audio: Read less than stream lenght");
        mData = nullptr;
        operator delete(contents);
        return;
    };

    mData = context->createBuffer(contents, (size_t)streamSize);
    if (mData == nullptr)
    {
        CUBOS_ERROR("Couldn't create audio buffer");
    }
    operator delete(contents);
}

cubos::engine::Audio::~Audio()
{
    // no need to delete buffer thanks to shared_ptr
}

cubos::engine::Audio::Audio(Audio&& other) noexcept
    : mData(other.mData)
{
    other.mData = nullptr;
}
