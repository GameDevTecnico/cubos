/// @file
/// @brief Component @ref cubos::engine::AudioListener.
/// @ingroup audio-plugin

#pragma once

#include <map>

#include <cubos/core/al/audio_context.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which adds an AudioListener to the entitiy
    /// @ingroup audio-plugin
    struct CUBOS_ENGINE_API AudioListener
    {
        CUBOS_REFLECT;

        /// @brief Whether the listener is active or not.
        bool active{false};
    };
} // namespace cubos::engine
