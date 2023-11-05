/// @file
/// @brief Component @ref cubos::engine::AccumulatedCorrection.
/// @ingroup physics-plugin

#pragma once

#include <glm/glm.hpp>

namespace cubos::engine
{
    /// @brief Component which holds the corrections accumulated from the constraints solving.
    /// @ingroup physics-plugin
    struct [[cubos::component("cubos/accumulated_correction", VecStorage)]] AccumulatedCorrection
    {
        glm::vec3 vec = {0.0F, 0.0F, 0.0F}; ///< Accumulated correction.
    };
} // namespace cubos::engine
