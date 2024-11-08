#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/audio/audio.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::Audio)
{
    return core::reflection::Type::create("cubos::engine::Audio");
}

cubos::engine::Audio::Audio(const std::shared_ptr<cubos::core::al::AudioContext>& context, core::memory::Stream& stream)
{
    stream.seek(0, cubos::core::memory::SeekOrigin::End);
    size_t streamSize = stream.tell();
    void* contents = operator new(sizeof(char) * streamSize);
    stream.seek(0, cubos::core::memory::SeekOrigin::Begin);
    if (!stream.readExact((char*)contents, streamSize))
    {
        CUBOS_ERROR("Couldn't load audio: stream read failed");
        buffer = nullptr;
        operator delete(contents);
        return;
    }

    buffer = context->createBuffer(contents, (size_t)streamSize);
    if (buffer == nullptr)
    {
        CUBOS_ERROR("Couldn't create audio buffer");
    }
    operator delete(contents);
}

cubos::engine::Audio::~Audio() = default;

cubos::engine::Audio::Audio(Audio&& other) noexcept
    : buffer(std::move(other.buffer))
{
}
