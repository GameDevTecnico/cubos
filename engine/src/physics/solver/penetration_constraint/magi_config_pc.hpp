/// @file
/// @brief Resource @ref cubos::engine::MagiConfig.
/// @ingroup physics-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Resource which allows configuration over magic numbers in the solver plugin.
    /// @ingroup physics-plugin
    struct MagiConfigPC
    {
        CUBOS_REFLECT;

        float biasMax = -4.0f;

        float contactHertzMin = 30.0f;

        float kNormal = 0.0f;

        float kFriction = 0.0f;

        bool cmpTangentLenSq(float tangentLenSq)
        {
            return tangentLenSq > 1e-6 * 1e-6;
        };

        bool cmpRestitution(float restitution)
        {
            return restitution == 0.0f;
        };

        bool cmpNormalSpeed(float normalSpeed)
        {
            return normalSpeed > -0.01f;
        };

        bool cmpNormalImpulse(float normalImpulse)
        {
            return normalImpulse == 0.0f;
        };
    };

} // namespace cubos::engine
