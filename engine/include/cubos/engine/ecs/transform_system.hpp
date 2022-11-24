#ifndef CUBOS_ENGINE_ECS_TRANSFORM_SYSTEM_HPP
#define CUBOS_ENGINE_ECS_TRANSFORM_SYSTEM_HPP

#include <cubos/core/ecs/query.hpp>

#include <components/cubos/local_to_world.hpp>
#include <components/cubos/position.hpp>
#include <components/cubos/rotation.hpp>
#include <components/cubos/scale.hpp>

namespace cubos::engine::ecs
{
    /// Updates LocalToWorld components matrices based on the combination of Position, Rotation and
    /// Scale components present.
    void transformSystem(core::ecs::Query<LocalToWorld&, const Position*, const Rotation*, const Scale*> query);
} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_TRANSFORM_SYSTEM_HPP
