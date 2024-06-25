/// @file
/// @brief Component @ref cubos::engine::SpotShadowCaster.
/// @ingroup render-shadows-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/render/shadows/caster.hpp>

namespace cubos::engine
{
    /// @brief Component which enables shadow casting on a spot light.
    /// @ingroup render-shadows-plugin
    struct CUBOS_ENGINE_API SpotShadowCaster
    {
        CUBOS_REFLECT;

        cubos::engine::ShadowCaster baseSettings;
    };
} // namespace cubos::engine
