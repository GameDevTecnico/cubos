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
    struct PhysicsConstantsPC
    {
        CUBOS_REFLECT;

        float biasMax = -4.0F;

        float contactHertzMin = 30.0F;

        float kNormal = 0.0F;

        float kFriction = 0.0F;

        static bool cmpTangentLenSq(float tangentLenSq)
        {
            return tangentLenSq > 1e-6 * 1e-6;
        };

        static bool cmpRestitution(float restitution)
        {
            return restitution == 0.0F;
        };

        static bool cmpNormalSpeed(float normalSpeed)
        {
            return normalSpeed > -0.01F;
        };

        static bool cmpNormalImpulse(float normalImpulse)
        {
            return normalImpulse == 0.0F;
        };
    };

} // namespace cubos::engine
