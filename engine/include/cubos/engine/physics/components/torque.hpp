/// @file
/// @brief Component @ref cubos::engine::Torque.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which holds torque applied on a body.
    /// @note Should be used with @ref AngularVelocity.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API Torque
    {
        CUBOS_REFLECT;

        glm::vec3 vec() const
        {
            return mTorque;
        }

        void add(glm::vec3 torque)
        {
            mTorque += torque;
        }

        void clear()
        {
            mTorque = {0.0F, 0.0F, 0.0F};
        }

    private:
        glm::vec3 mTorque = {0.0F, 0.0F, 0.0F};
    };

} // namespace cubos::engine
