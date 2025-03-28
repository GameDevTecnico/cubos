/// @file
/// @brief Class @ref cubos::core::ecs::CommandBuffer.
/// @ingroup core-ecs

#pragma once

#include <mutex>
#include <unordered_map>
#include <vector>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/memory/function.hpp>

namespace cubos::core::reflection
{
    class Type;
}

namespace cubos::core::memory
{
    class AnyValue;
}

namespace cubos::core::ecs
{
    class World;
    class Blueprint;

    /// @brief Stores commands to execute them later.
    /// @ingroup core-ecs
    class CUBOS_CORE_API CommandBuffer final
    {
    public:
        /// @brief Constructs.
        /// @param world World to which the commands will be applied.
        CommandBuffer(World& world);

        /// @brief Inserts a resource into the world.
        /// @param value Resource value.
        void insertResource(memory::AnyValue value);

        /// @brief Removes a resource from the world.
        /// @param type Resource type.
        void eraseResource(const reflection::Type& type);

        /// @brief Creates a new entity.
        /// @return Entity identifier.
        Entity create();

        /// @brief Destroys an entity.
        /// @param entity Entity identifier.
        void destroy(Entity entity);

        /// @brief Spawns a blueprint into the world.
        /// @param blueprint Blueprint to spawn.
        /// @return Map of entity names to their identifiers.
        std::unordered_map<std::string, Entity> spawn(const Blueprint& blueprint);

        /// @brief Adds a component to an entity.
        /// @param entity Entity identifier.
        /// @param type Component type.
        /// @param value Component value to be moved.
        void add(Entity entity, const reflection::Type& type, void* value);

        /// @brief Removes a component from an entity.
        /// @param entity Entity identifier.
        /// @param type Component type.
        void remove(Entity entity, const reflection::Type& type);

        /// @brief Inserts a relation between the two given entities.
        ///
        /// If the relation already exists, it is overwritten.
        ///
        /// @param from From entity.
        /// @param to To entity.
        /// @param type Relation type.
        /// @param value Relation value to move.
        void relate(Entity from, Entity to, const reflection::Type& type, void* value);

        /// @brief Removes the relation, if there's any, between the two given entities.
        /// @param from From entity.
        /// @param to To entity.
        /// @param type Relation type.
        void unrelate(Entity from, Entity to, const reflection::Type& type);

        /// @brief Pushes a command.
        /// @param command Command function.
        void push(memory::Function<void(World&)> command);

        /// @brief Commits the commands to the world.
        void commit();

    private:
        std::mutex mMutex;                                     ///< Protects the commands buffer.
        World& mWorld;                                         ///< World to which the commands will be applied.
        std::vector<memory::Function<void(World&)>> mCommands; ///< Commands to be executed.
    };
} // namespace cubos::core::ecs
