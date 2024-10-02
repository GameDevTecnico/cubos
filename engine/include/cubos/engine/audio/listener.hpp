/// @file
/// @brief Component @ref cubos::engine::AudioListener.
/// @ingroup audio-plugin

#pragma once

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

        bool active = false; //< Is the listener active?

    private:
        cubos::core::al::Listener listener;
    };
} // namespace cubos::engine