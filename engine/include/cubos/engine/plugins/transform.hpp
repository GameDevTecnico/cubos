#pragma once

#include <cubos/engine/cubos.hpp>

namespace cubos::engine::plugins
{
    /// Plugin which adds components to represent the transform of an entity.
    ///
    /// @details This plugin adds one system, which updates the `LocalToWorld` component of all
    /// entities with a `LocalTransform` component, taking information from the `Position`,
    /// `Rotation` and `Scale` components. These three components are optional, and any subset of
    /// them can be used.
    ///
    /// Components:
    /// - `LocalToWorld`: holds the local to world transform matrix.
    /// - `Position`: holds the position of the entity.
    /// - `Rotation`: holds the rotation of the entity.
    /// - `Scale`: holds the scale of the entity.
    ///
    /// Tags:
    /// - `cubos.transform.update`: updates the `LocalToWorld` components with the information from
    ///   the `Position`, `Rotation` and `Scale` components.
    ///
    /// @param cubos CUBOS. main class
    void transformPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins
