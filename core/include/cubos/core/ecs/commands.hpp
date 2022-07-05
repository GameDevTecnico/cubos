#ifndef CUBOS_CORE_ECS_COMMANDS_HPP
#define CUBOS_CORE_ECS_COMMANDS_HPP

#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    /// Object responsible for storing ECS commands to execute them later.
    class Commands final
    {
    public:
        Commands() = default;
        ~Commands() = default;

        /// Adds components to an entity.
        /// @tparam ComponentTypes The types of the components to be added.
        /// @param entity The entity to which the components will be added.
        /// @param components The components to add.
        template <typename... ComponentTypes> void addComponents(Entity entity, ComponentTypes... components);

        /// Removes components from an entity.
        /// @tparam ComponentTypes The types of the components to be removed.
        /// @param entity The entity from which the components will be removed.
        template <typename... ComponentTypes> void removeComponents(Entity entity);

        /// Creates a new entity with the given components.
        /// @tparam ComponentTypes The types of the components to be added.
        /// @param components The components to add.
        /// @returns The new entity.
        template <typename... ComponentTypes> Entity create(ComponentTypes... components);

        /// Destroys an entity.
        /// @param entity The entity to destroy.
        void destroy(Entity entity);

    private:

    };
} // namespace cubos::core::ecs

#endif // CUBOS_CORE_ECS_COMMANDS_HPP
