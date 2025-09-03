#pragma once

#include <cubos/engine/api.hpp>
#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    /// @brief Tag for particle system updates.
    /// @ingroup render-particles-plugin
    CUBOS_ENGINE_API extern Tag setupParticleSystemTag;

    /// @brief Tag for particle buffer setup.
    /// @ingroup render-particles-plugin
    CUBOS_ENGINE_API extern Tag setupBuffersTag;

    /// @brief Tag for particle system rendering.
    /// @ingroup render-particles-plugin
    CUBOS_ENGINE_API extern Tag particleSystemTag;

    /// @brief Plugin entry function.
    /// @param cubos @b Cubos main class.
    /// @ingroup render-particles-plugin
    CUBOS_ENGINE_API void particleSystemPlugin(Cubos& cubos);
} // namespace cubos::engine