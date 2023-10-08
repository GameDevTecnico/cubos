/// @file
/// @brief Component @ref cubos::engine::LocalToWorld.
/// @ingroup transform-plugin

#pragma once

#include <glm/mat4x4.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::engine
{
    /// @brief Component which stores the transformation matrix of an entity, from local to world
    /// space.
    ///
    /// @note This component is written to by the @ref transform-plugin "transform plugin", and
    /// it only makes sense to modify it manually if its not accompanied by the other transform
    /// components.
    ///
    /// @sa Position Applies a translation to this matrix.
    /// @sa Rotation Applies a rotation to this matrix.
    /// @sa Scale Applies a scaling to this matrix.
    /// @ingroup transform-plugin
    struct [[cubos::component("cubos/local_to_world", VecStorage)]] LocalToWorld
    {
        CUBOS_REFLECT;

        glm::mat4 mat = glm::mat4(1.0F); ///< Local to world space matrix.
    };

} // namespace cubos::engine
