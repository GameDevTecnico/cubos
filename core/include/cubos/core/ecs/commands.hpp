#ifndef CUBOS_CORE_ECS_COMMANDS_HPP
#define CUBOS_CORE_ECS_COMMANDS_HPP

#include <cubos/core/ecs/world.hpp>

#include <unordered_map>
#include <mutex>

namespace cubos::core::ecs
{
    /// Object responsible for storing ECS commands to execute them later.
    class Commands final
    {
    public:
        /// @param world The world to which the commands will be applied.
        Commands(World& world);
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
        /// Commits the commands to the world.
        void commit();

        World& world;     ///< The world to which the commands will be applied.
        std::mutex mutex; ///< Protects the world.
    };
} // namespace cubos::core::ecs

#endif // CUBOS_CORE_ECS_COMMANDS_HPP
