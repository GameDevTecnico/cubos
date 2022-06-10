#ifndef CUBOS_CORE_ECS_ITERATING_SYSTEM_HPP
#define CUBOS_CORE_ECS_ITERATING_SYSTEM_HPP

#include <cinttypes>

#include <cubos/core/ecs/system.hpp>
#include <cubos/core/ecs/world_view.hpp>

namespace cubos::core::ecs
{
    /// Base class for all systems that iterate over entities.
    /// @tparam ComponentTypes The set of component types to be iterated.
    template <typename... ComponentTypes> class IteratingSystem : public System
    {
    public:
        IteratingSystem() = default;
        virtual ~IteratingSystem() override = default;

        // Method implementations.

        void update(World& world) override;

    protected:
        /// Called every frame of the engine.
        /// The system logic should be implemented in this function.
        ///
        /// @details For example, if you want to add the velocity of every entity to its
        /// position every frame, you would do it in this function.
        /// @param world The world the system is added to.
        /// @param entity The entity to be processed.
        /// @param components The components of the entity being viewed.
        virtual void process(World& world, uint64_t entity, ComponentTypes&... components) = 0;
    };

    // Implementation.

    template <typename... ComponentTypes> inline void IteratingSystem<ComponentTypes...>::update(World& world)
    {
        for (auto entity : WorldView<ComponentTypes...>(world))
        {
            process(world, entity, world.getComponent<ComponentTypes>(entity)...);
        }
    }
} // namespace cubos::core::ecs

#endif // CUBOS_CORE_ECS_ITERATING_SYSTEM_HPP
