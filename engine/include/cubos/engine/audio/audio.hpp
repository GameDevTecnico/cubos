/// @file
/// @brief Class @ref cubos::engine::Audio.
/// @ingroup audio-plugin

#pragma once

#include <cubos/core/al/miniaudio_context.hpp>
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

        /// @brief Audio buffer.
        cubos::core::al::Buffer buffer;

        explicit Audio(const std::shared_ptr<cubos::core::al::AudioContext>& context, core::memory::Stream& stream);
        Audio(Audio&& other) noexcept;
        ~Audio();
    };
} // namespace cubos::engine
