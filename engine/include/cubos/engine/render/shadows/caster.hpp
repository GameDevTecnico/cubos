/// @file
/// @brief Struct @ref cubos::engine::ShadowCaster.
/// @ingroup render-shadows-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Struct which contains the settings common to all shadow casters.
    /// @ingroup render-shadows-plugin
    struct CUBOS_ENGINE_API ShadowCaster
    {
        CUBOS_REFLECT;

        float bias = 0.002F;     ///< Shadow bias.
        float blurRadius = 1.0F; ///< Shadow blur radius.
        int id = -1;             ///< Caster id for the shadow atlas.
    };
} // namespace cubos::engine
