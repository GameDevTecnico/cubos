/// @file
/// @brief Component @ref cubos::engine::AudioSourceImpl.
/// @ingroup audio-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/al/audio_context.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/audio/audio.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @brief Component which adds an AudioSource to the entitiy
    /// @ingroup audio-plugin
    struct AudioSourceImpl
    {
        CUBOS_REFLECT;

        cubos::core::al::Source source;
        Asset<Audio> sound{};
    };
} // namespace cubos::engine
