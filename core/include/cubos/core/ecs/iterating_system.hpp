#ifndef CUBOS_CORE_ECS_ITERATING_SYSTEM_HPP
#define CUBOS_CORE_ECS_ITERATING_SYSTEM_HPP

#include <cinttypes>

#include <cubos/core/ecs/system.hpp>
#include <cubos/core/ecs/world_view.hpp>

namespace cubos::core::ecs
{
    template <typename... ComponentTypes> class IteratingSystem : public System
    {
    public:
        void update(World& world) override;
        virtual void process(World& world, uint64_t entity) = 0;
    };

    template <typename... ComponentTypes> void IteratingSystem<ComponentTypes...>::update(World& world)
    {
        for (auto entity : WorldView<ComponentTypes...>(world))
        {
            process(world, entity);
        }
    }
} // namespace cubos::core::ecs

#endif // CUBOS_CORE_ECS_ITERATING_SYSTEM_HPP