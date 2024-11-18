/// @file
/// @brief Resource @ref cubos::engine::MagiConfig.
/// @ingroup physics-plugin

#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which allows configuration over magic numbers in the solver plugin.
    /// @ingroup physics-plugin
    struct MagiConfigIntegration
    {
        CUBOS_REFLECT;

        bool cmpInvMass(float inverseMass)
        {
            return inverseMass <= 0.0f;
        };

        bool cmpInvInertia(glm::mat3 inverseInertia)
        {
            return inverseInertia == glm::mat3(0.0f);
        };
    };

} // namespace cubos::engine
