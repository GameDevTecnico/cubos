/// @file
/// @brief Component @ref cubos::engine::SpotShadowCaster.
/// @ingroup render-shadow-casters-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/render/shadows/casters/caster.hpp>

namespace cubos::engine
{
    /// @brief Component which enables shadow casting on a spot light.
    /// @ingroup render-shadow-casters-plugin
    struct CUBOS_ENGINE_API SpotShadowCaster
    {
        CUBOS_REFLECT;

        cubos::engine::ShadowCaster baseSettings;
    };
} // namespace cubos::engine
