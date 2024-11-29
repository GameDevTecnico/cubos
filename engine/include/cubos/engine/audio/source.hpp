/// @file
/// @brief Component @ref cubos::engine::AudioSource.
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
    /// @brief Component which makes an entity emit sound.
    ///
    /// The behavior of this component can be controlled through the @ref AudioPlay, AudioStop and AudioPause
    /// components. Take a look at the @ref audio sample for more information.
    ///
    /// If the cone angles are changed, the audio source becomes directional, in which case it points in the direction
    /// specified by @ref LocalToWorld::forward.
    /// @ingroup audio-plugin
    struct CUBOS_ENGINE_API AudioSource
    {
        CUBOS_REFLECT;

        /// @brief Whether the source is playing or not.
        bool playing{false};

        /// @brief Whether the source should loop sounds instead of stopping.
        bool looping{false};

        /// @brief Gain of the source.
        float gain{1.0F};

        /// @brief Pitch of the source.
        float pitch{1.0F};

        /// @brief Maximum distance at which the source is heard.
        float maxDistance{FLT_MAX};

        /// @brief Minimum distance at which the source is heard.
        float minDistance{1.0F};

        /// @brief Inner cone angle of the source.
        float innerConeAngle{360.0F};

        /// @brief Outer cone angle of the source.
        float outerConeAngle{360.0F};

        /// @brief Outer cone gain of the source.
        float outerConeGain{1.0F};

        /// @brief Sound to be played by the source.
        Asset<Audio> sound{};
    };
} // namespace cubos::engine
