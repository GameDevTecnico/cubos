/// @file
/// @brief Component @ref cubos::engine::OrthographicCamera
/// @ingroup render-camera-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component which defines parameters of a orthographic camera used to render the world.
    /// @note Should be used with @ref LocalToWorld.
    /// @ingroup render-camera-plugin
    struct CUBOS_ENGINE_API OrthographicCamera
    {
        CUBOS_REFLECT;

        /// @brief The axis that can be fixed for this projection.
        enum class Axis
        {
            Horizontal,
            Vertical
        };

        /// @brief The size of the fixed axis of the projection.
        float size{50.0F};

        /// @brief The axis to be fixed for the projection.
        Axis axis{Axis::Vertical};

        /// @brief Near clipping plane.
        float zNear{0.1F};

        /// @brief Far clipping plane.
        float zFar{1000.0F};
    };
} // namespace cubos::engine

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_ENGINE_API, cubos::engine::OrthographicCamera::Axis);
