/// @file
/// @brief Class @ref cubos::core::ecs::Commands.
/// @ingroup core-ecs-system-arguments

#pragma once

#include <unordered_map>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/system/fetcher.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::ecs
{
    class CommandBuffer;
    class Blueprint;

    /// @brief System argument used to write ECS commands and execute them at a later time.
    ///
    /// Internally wraps a reference to a CommandBuffer object.
    ///
    /// @ingroup core-ecs-system-arguments
    class Commands final
    {
    public:
        /// @brief Allows editing an entity created by a @ref Commands object.
        class EntityBuilder;

        /// @brief Allows editing a blueprint spawned by a @ref Commands object.
        class BlueprintBuilder;

        /// @brief Constructs.
        /// @param buffer Command buffer to write to.
        Commands(CommandBuffer& buffer);

        /// @brief Move constructor.
        Commands(Commands&&) = default;

        /// @brief Creates a new entity.
        /// @return Builder which can be used to modify the created entity.
        EntityBuilder create();

        /// @brief Destroys an entity.
        /// @param entity Entity identifier.
        void destroy(Entity entity);

        /// @brief Spawns a blueprint into the world.
        /// @param blueprint Blueprint to spawn.
        /// @return Blueprint builder.
        BlueprintBuilder spawn(const Blueprint& blueprint);

        /// @brief Adds a component to an entity.
        /// @param entity Entity identifier.
        /// @param type Component type.
        /// @param value Component value.
        /// @return Commands.
        Commands& add(Entity entity, const reflection::Type& type, void* value);

        /// @brief Adds a component to an entity.
        /// @tparam T Component type.
        /// @param entity Entity identifier.
        /// @param value Component value.
        /// @return Commands.
        template <reflection::Reflectable T>
        Commands& add(Entity entity, T value)
        {
            return this->add(entity, reflection::reflect<T>(), &value);
        }

        /// @brief Removes a component from an entity.
        /// @param entity Entity identifier.
        /// @param type Component type.
        /// @return Commands.
        Commands& remove(Entity entity, const reflection::Type& type);

        /// @brief Removes a component from an entity.
        /// @tparam T Component type.
        /// @param entity Entity identifier.
        /// @return Commands.
        template <reflection::Reflectable T>
        Commands& remove(Entity entity)
        {
            return this->remove(entity, reflection::reflect<T>());
        }

        /// @brief Inserts a relation between the two given entities.
        ///
        /// If the relation already exists, it is overwritten.
        ///
        /// @param from From entity.
        /// @param to To entity.
        /// @param type Relation type.
        /// @param value Relation value to move.
        /// @return Commands.
        Commands& relate(Entity from, Entity to, const reflection::Type& type, void* value);

        /// @brief Inserts a relation between the two given entities.
        ///
        /// If the relation already exists, it is overwritten.
        ///
        /// @tparam T Relation type.
        /// @param from From entity.
        /// @param to To entity.
        /// @param value Relation value.
        /// @return Commands.
        template <reflection::Reflectable T>
        Commands& relate(Entity from, Entity to, T value)
        {
            return this->relate(from, to, reflection::reflect<T>(), &value);
        }

        /// @brief Removes the relation, if there's any, between the two given entities.
        /// @param from From entity.
        /// @param to To entity.
        /// @param type Relation type.
        /// @return Commands.
        Commands& unrelate(Entity from, Entity to, const reflection::Type& type);

        /// @brief Removes the relation, if there's any, between the two given entities.
        /// @tparam T Relation type.
        /// @param from From entity.
        /// @param to To entity.
        /// @return Commands.
        template <reflection::Reflectable T>
        Commands& unrelate(Entity from, Entity to)
        {
            return this->unrelate(from, to, reflection::reflect<T>());
        }

    private:
        CommandBuffer& mBuffer; ///< Command buffer to write to.
    };

    class Commands::EntityBuilder final
    {
    public:
        /// @brief Constructs.
        /// @param buffer Command buffer.
        /// @param entity Entity being modified.
        EntityBuilder(CommandBuffer& buffer, Entity entity);

        /// @brief Gets the entity this builder is modifying.
        /// @return Entity identifier.
        Entity entity() const;

        /// @brief Adds a component to the entity.
        /// @param type Component type.
        /// @param value Component value.
        /// @return Reference to this builder, for chaining.
        EntityBuilder& add(const reflection::Type& type, void* value);

        /// @brief Adds a component to the entity.
        /// @tparam T Component type.
        /// @param value Component value.
        /// @return Reference to this builder, for chaining.
        template <reflection::Reflectable T>
        EntityBuilder& add(T value)
        {
            return this->add(reflection::reflect<T>(), &value);
        }

    private:
        CommandBuffer& mBuffer;
        Entity mEntity;
    };

    class Commands::BlueprintBuilder final
    {
    public:
        /// @brief Constructs.
        /// @param map Map of entity names to the instantiated entities.
        /// @param commands Commands object that created this entity.
        BlueprintBuilder(CommandBuffer& buffer, std::unordered_map<std::string, Entity> nameToEntity);

        /// @brief Gets an entity of the spawned blueprint.
        ///
        /// Aborts if the name does not match any entity of the blueprint.
        ///
        /// @param name Entity name.
        /// @return Entity identifier.
        Entity entity(const std::string& name) const;

        /// @brief Adds a component to an entity of the blueprint.
        ///
        /// Aborts if @p name does not match any entity of the blueprint.
        ///
        /// @param name Entity name.
        /// @param type Component Type.
        /// @param value Component value.
        /// @return Reference to this builder, for chaining.
        BlueprintBuilder& add(const std::string& name, const reflection::Type& type, void* value);

        /// @brief Adds a component to an entity of the blueprint.
        ///
        /// Aborts if @p name does not match any entity of the blueprint.
        ///
        /// @tparam T Component type.
        /// @param name Entity name.
        /// @param value Component value.
        /// @return Reference to this builder, for chaining.
        template <reflection::Reflectable T>
        BlueprintBuilder& add(const std::string& name, T value)
        {
            return this->add(name, reflection::reflect<T>(), &value);
        }

    private:
        CommandBuffer& mBuffer;
        std::unordered_map<std::string, Entity> mNameToEntity;
    };

    template <>
    class SystemFetcher<Commands>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher(World& /*world*/, const SystemOptions& /*options*/)
        {
        }

        void analyze(SystemAccess& /*access*/) const
        {
        }

        static Commands fetch(CommandBuffer& cmdBuffer)
        {
            return {cmdBuffer};
        }
    };
} // namespace cubos::core::ecs
