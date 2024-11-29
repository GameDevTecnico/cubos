/// @file
/// @brief Component @ref cubos::engine::AudioPause.
/// @ingroup audio-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @brief Component which triggers the pause of an audio source.
    /// @ingroup audio-plugin
    struct CUBOS_ENGINE_API AudioPause
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine
