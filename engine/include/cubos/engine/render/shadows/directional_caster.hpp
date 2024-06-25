/// @file
/// @brief Component @ref cubos::engine::DirectionalShadowCaster.
/// @ingroup render-shadows-plugin

#pragma once

#include <vector>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/render/shadows/caster.hpp>

namespace cubos::engine
{
    /// @brief Component which enables shadow casting on a directional light.
    /// @ingroup render-shadows-plugin
    struct CUBOS_ENGINE_API DirectionalShadowCaster
    {
        CUBOS_REFLECT;

        cubos::engine::ShadowCaster baseSettings;

        int numSplits;                     ///< Number of splits for PSSM/CSM.
        std::vector<float> splitDistances; ///< Distance of each split from 0 to 1.
        float maxDistance = 0;             ///< Max distance (if 0, uses camera zFar).
    };
} // namespace cubos::engine
