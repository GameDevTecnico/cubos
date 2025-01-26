/// @file
/// @brief Class @ref cubos::core::geom::Frustum.
/// @ingroup core-geom

#include <cubos/core/api.hpp>
#include <cubos/core/geom/plane.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::geom
{
    /// @brief Represents a camera frustum.
    /// @ingroup core-geom
    struct CUBOS_CORE_API Frustum
    {
        CUBOS_REFLECT;

        /// @brief Top plane of Frustum.
        Plane top;

        /// @brief Right plane of Frustum.
        Plane right;

        /// @brief Bottom plane of Frustum.
        Plane bottom;

        /// @brief Left plane of Frustum.
        Plane left;

        /// @brief Near plane of Frustum.
        Plane near;

        /// @brief Far plane of Frustum.
        Plane far;
    };
} // namespace cubos::core::geom