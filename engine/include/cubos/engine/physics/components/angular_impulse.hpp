/// @file
/// @brief Component @ref cubos::engine::AngularImpulse.
/// @ingroup physics-plugin

#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which holds angular impulses applied on a body.
    /// @note Should be used with @ref AngularVelocity.
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API AngularImpulse
    {
        CUBOS_REFLECT;

        glm::vec3 vec() const
        {
            return mAngularImpulse;
        }

        void add(glm::vec3 angularImpulse)
        {
            mAngularImpulse += angularImpulse;
        }

        void clear()
        {
            mAngularImpulse = {0.0F, 0.0F, 0.0F};
        }

    private:
        glm::vec3 mAngularImpulse;
    };

} // namespace cubos::engine
