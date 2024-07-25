/// @file
/// @brief Component @ref cubos::engine::PhysicsMaterial.
/// @ingroup physics-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which defines the friction and bounciness of a body.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API PhysicsMaterial
    {
        CUBOS_REFLECT;

        enum class MixProperty
        {
            Maximum,
            Multiply,
            Minimum,
            Average
        }; ///< Type of mixing for the material property. When mixing between bodies with different types, the priority
           ///< is taken into account. The priority is as defined in the enum, lower values will take priority over
           ///< higher values.

        float friction = 0.0F;   ///< Static and Dynamic friction coefficient for the body. Range between 0, for no
                                 ///< friction, and 1, for maximum friction.
        float bounciness = 0.0F; ///< Bounciness of the body. Range between 0, for no bounce, and 1, for maximum bounce.
        MixProperty frictionMix =
            MixProperty::Average; ///< Type of value mixing for friction, according to @ref MixProperty.
        MixProperty bouncinessMix =
            MixProperty::Average; ///< Type of value mixing for bounciness, according to @ref MixProperty.
    };
} // namespace cubos::engine
