/// @file
/// @brief Component @ref cubos::engine::FXAA.
/// @ingroup render-tone-mapping-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the FXAA configuration for a render target.
    /// @ingroup render-tone-mapping-plugin
    struct CUBOS_ENGINE_API FXAA
    {
        CUBOS_REFLECT;

        /// @brief Edge threshold's min value.
        float edgeThresholdMin = 0.0312F;

        /// @brief Edge threshold's max value.
        float edgeThresholdMax = 0.125F;

        /// @brief Subpixel quality value.
        float subpixelQuality = 0.75F;

        /// @brief Edges exploration iteration value.
        int iterations = 12;
    };
} // namespace cubos::engine
