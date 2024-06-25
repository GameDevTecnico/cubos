/// @file
/// @brief Component @ref cubos::engine::PointShadowCaster.
/// @ingroup render-shadows-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/render/shadows/caster.hpp>

namespace cubos::engine
{
    /// @brief Component which enables shadow casting on a point light.
    /// @ingroup render-shadows-plugin
    struct CUBOS_ENGINE_API PointShadowCaster
    {
        CUBOS_REFLECT;

        cubos::engine::ShadowCaster baseSettings;
    };
} // namespace cubos::engine
