/// @dir
/// @brief @ref transform-plugin plugin directory.

/// @file
/// @brief Plugin entry point.
/// @ingroup transform-plugin

#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/transform/local_to_parent.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/transform/rotation.hpp>
#include <cubos/engine/transform/scale.hpp>
#include <cubos/core/ecs/tag.hpp>

namespace cubos::core::ecs {
    
}
namespace cubos::engine
{
    /// @defgroup transform-plugin Transform
    /// @ingroup engine
    /// @brief Adds transform components which assign positions, rotations and scaling to entities.
    ///
    /// This plugin operates on entities with @ref LocalToWorld and  @ref LocalToParent components, and any combination
    /// of the @ref Position, @ref Rotation and @ref Scale components. For example, if you have an entity which doesn't
    /// need rotation, but has a position and a scale, you do not need to add the @ref Rotation component, and its
    /// transform will still be updated.
    ///
    /// @note Any entity with either a @ref Position, @ref Rotation or @ref Scale component
    /// automatically gets a @ref LocalToWorld component.
    ///
    /// ## Components
    /// - @ref LocalToParent - holds the local to parent transform matrix.
    /// - @ref LocalToWorld - holds the local to world transform matrix.
    /// - @ref Position - holds the position of an entity.
    /// - @ref Rotation - holds the rotation of an entity.
    /// - @ref Scale - holds the scaling of an entity.
    ///
    /// ## Relations
    /// - @ref ChildOf - tree like relation which indicates an entity is a child of another.

    /// @brief the @ref LocalToWorld components are added to entities with @ref
    /// Position, @ref Rotation or @ref Scale components.
    extern Tag TransformMissingLocalTag;

    /// @brief the @ref Position, @ref Rotation, @ref Scale and possibly @ref LocalToParent
    /// components are added to entities with @ref LocalToWorld components.
    extern Tag TransformMissingTag;

    /// @brief the @ref LocalToWorld or @ref LocalToParent components are updated with
    /// the information from the @ref Position, @ref Rotation and @ref Scale components.
    extern Tag TransformUpdateRelativeTag;

    /// @brief the @ref LocalToWorld components are updated with the information from the @ref
    /// LocalToParent component and the @ref LocalToWorld components of the parent.
    extern Tag TransformUpdateTag;

    extern Tag TransformUpdatePropagateTag;

    /// @brief Plugin entry function.
    /// @param cubos @b CUBOS. main class
    /// @ingroup transform-plugin
    void transformPlugin(Cubos& cubos);
} // namespace cubos::engine
