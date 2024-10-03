/// @file
/// @brief Component @ref cubos::engine::AudioSource.
/// @ingroup audio-plugin

#pragma once

#include <cubos/core/al/audio_context.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which adds an AudioSource to the entitiy
    /// @ingroup audio-plugin
    struct CUBOS_ENGINE_API AudioSource
    {
        CUBOS_REFLECT;

    private:
        cubos::core::al::Source source;
    };
} // namespace cubos::engine