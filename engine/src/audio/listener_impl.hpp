/// @file
/// @brief Component @ref cubos::engine::AudioListenerImpl.
/// @ingroup audio-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @brief Component which adds an AudioListener to the entitiy
    /// @ingroup audio-plugin
    struct AudioListenerImpl
    {
        CUBOS_REFLECT;

        Opt<std::size_t> index{};
    };
} // namespace cubos::engine
