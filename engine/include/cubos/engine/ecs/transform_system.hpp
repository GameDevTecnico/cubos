#ifndef CUBOS_ENGINE_ECS_TRANSFORM_SYSTEM_HPP
#define CUBOS_ENGINE_ECS_TRANSFORM_SYSTEM_HPP

#include <cubos/core/ecs/query.hpp>

#include <cubos/engine/ecs/local_to_world.hpp>
#include <cubos/engine/ecs/position.hpp>
#include <cubos/engine/ecs/rotation.hpp>
#include <cubos/engine/ecs/scale.hpp>

namespace cubos::engine::ecs
{
    /// Updates LocalToWorld components matrices based on the combination of Position, Rotation and
    /// Scale components present.
    void transformSystem(core::ecs::Query<LocalToWorld&, const Position*, const Rotation*, const Scale*> query);
} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_TRANSFORM_SYSTEM_HPP
