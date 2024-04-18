/// @file
/// @brief Component @ref cubos::engine::ToneMapping.
/// @ingroup render-tone-mapping-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which stores Tone Mapping configuration for a render target.
    /// @ingroup render-tone-mapping-plugin
    struct CUBOS_ENGINE_API ToneMapping
    {
        CUBOS_REFLECT;

        /// @brief Gamma value.
        float gamma = 2.2F;

        /// @brief Exposure value.
        float exposure = 1.0F;
    };
} // namespace cubos::engine
