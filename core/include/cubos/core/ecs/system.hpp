#ifndef CUBOS_CORE_ECS_SYSTEM_HPP
#define CUBOS_CORE_ECS_SYSTEM_HPP

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// Base class for all systems.
    class System
    {
    public:
        System() = default;
        virtual ~System() = default;

        /// Called when the system is added to the world.
        /// You can use this, for example, to create entities.
        /// @param world The world the system is added to.
        virtual void init(World& world);

        /// Called every frame of the engine.
        /// The system logic should be implemented in this function.
        ///
        /// @details For example, if you want to add the velocity of every entity to its
        /// position every frame, you would do it in this function.
        /// @param world The world the system is added to.
        virtual void update(World& world);
    };
} // namespace cubos::core::ecs
#endif // CUBOS_CORE_ECS_SYSTEM_HPP
