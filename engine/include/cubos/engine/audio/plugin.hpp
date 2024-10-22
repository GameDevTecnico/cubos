/// @dir
/// @brief @ref audio-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup audio-plugin

#pragma once

#include <cubos/engine/audio/audio.hpp>
#include <cubos/engine/audio/listener.hpp>
#include <cubos/engine/audio/source.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @defgroup audio-plugin Audio
    /// @ingroup engine
    /// @brief Adds audio to @b Cubos
    ///
    /// ## Settings
    /// - `audio.listener.count` - number of listeners per audio device (default: `4`).
    ///
    /// ## Dependencies
    /// - @ref settings-plugin

    /// @brief Initializes the audio context (after @ref settingsTag).
    CUBOS_ENGINE_API extern Tag audioInitTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup assets-plugin
    CUBOS_ENGINE_API void audioPlugin(Cubos& cubos);
} // namespace cubos::engine
