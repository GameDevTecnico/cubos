#ifndef CUBOS_ENGINE_ECS_TRANSFORM_SYSTEM_HPP
#define CUBOS_ENGINE_ECS_TRANSFORM_SYSTEM_HPP

#include <cubos/core/ecs/iterating_system.hpp>

#include <cubos/engine/ecs/local_to_world.hpp>

namespace cubos::engine::ecs
{
    /// Updates LocalToWorld components matrices based on the combination of Position, Rotation and
    /// Scale components present.
    class TransformSystem : public core::ecs::IteratingSystem<LocalToWorld>
    {
    private:
        void process(core::ecs::World& world, uint64_t entity, LocalToWorld& localToWorld) override;
    };
} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_TRANSFORM_SYSTEM_HPP
