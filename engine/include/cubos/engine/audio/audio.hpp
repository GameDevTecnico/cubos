/// @file
/// @brief Class @ref cubos::engine::Audio.
/// @ingroup audio-plugin
#pragma once

#include <cubos/core/al/miniaudio_device.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>

namespace cubos::engine
{
    /// @brief Asset containing raw Audio data.
    ///
    /// @ingroup audio-plugin
    struct CUBOS_ENGINE_API Audio
    {
        CUBOS_REFLECT;
        std::shared_ptr<cubos::core::al::impl::Buffer> mData; // Raw data of the audio
        size_t mLength;                                       // Audio length in seconds TODO: add getter in audio

        explicit Audio(std::shared_ptr<cubos::core::al::MiniaudioDevice> device, core::memory::Stream& stream);
        Audio(Audio&& other) noexcept;
        ~Audio();
    };
} // namespace cubos::engine
