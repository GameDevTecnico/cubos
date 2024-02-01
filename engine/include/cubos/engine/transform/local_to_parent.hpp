/// @file
/// @brief Component @ref cubos::engine::LocalToParent.
/// @ingroup transform-plugin

#pragma once

#include <glm/mat4x4.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the transformation matrix of an entity, from local to parent
    /// space.
    ///
    /// @note This component is written to by the @ref transform-plugin "transform plugin", thus
    /// it never makes sense to write to it manually.
    ///
    /// @sa @ref Position, @ref Rotation and @ref Scale, which apply transformations to this matrix.
    /// @ingroup transform-plugin
    struct LocalToParent
    {
        CUBOS_REFLECT;

        glm::mat4 mat = glm::mat4(1.0F); ///< Local to parent space matrix.
    };

} // namespace cubos::engine
