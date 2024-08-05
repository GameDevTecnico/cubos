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

        /// @brief Type of mixing for the material property. When mixing between bodies with different types, the
        /// priority is taken into account. The priority is as defined in the enum, lower priority takes precedence.
        enum class MixProperty
        {
            Maximum,  /**< Use highest property value | Priority 0. */
            Multiply, /**< Multiply property values | Priority 1. */
            Minimum,  /**< Use lowest property value | Priority 2. */
            Average   /**< Use the average of property values | Priority 3. */
        };

        /// @brief Static and Dynamic friction coefficient for the body. Range between 0, for no friction, and 1, for
        /// maximum friction.
        float friction = 0.0F;
        /// @brief Bounciness of the body. Range between 0, for no bounce, and 1, for maximum bounce.
        float bounciness = 0.0F;
        /// @brief Type of value mixing for friction, according to @ref MixProperty.
        MixProperty frictionMix = MixProperty::Average;
        /// @brief Type of value mixing for bounciness, according to @ref MixProperty.
        MixProperty bouncinessMix = MixProperty::Average;
    };
} // namespace cubos::engine

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, cubos::engine::PhysicsMaterial::MixProperty);
