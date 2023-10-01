/// @file
/// @brief Class @ref cubos::core::ecs::Commands and related types.
/// @ingroup core-ecs-system

#pragma once

#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::core::ecs
{
    class Blueprint;
    class CommandBuffer;
    class Dispatcher;

    /// @brief Allows editing an entity created by a @ref Commands object.
    /// @ingroup core-ecs-system
    class EntityBuilder final
    {
    public:
        ~EntityBuilder() = default;

        /// @brief Gets the entity this builder is editing.
        /// @return Entity identifier.
        Entity entity() const;

        /// @brief Gets a reference to a component of the entity.
        /// @tparam ComponentType Component type.
        /// @return Reference to the component.
        template <typename ComponentType>
        ComponentType& get();

        /// @brief Adds components to the entity.
        /// @tparam ComponentTypes Component types.
        /// @param components Components to add.
        /// @return Reference to this builder, for chaining.
        template <typename... ComponentTypes>
        EntityBuilder& add(ComponentTypes&&... components);

    private:
        friend CommandBuffer;

        /// @brief Constructs.
        /// @param entity Entity being edited.
        /// @param commands Commands object that created this entity.
        EntityBuilder(Entity entity, CommandBuffer& commands);

        Entity mEntity;           ///< Entity being edited.
        CommandBuffer& mCommands; ///< Commands object that created this entity.
    };

    /// @brief Used to edit a blueprint spawned by a @ref Commands object.
    /// @ingroup core-ecs-system
    class BlueprintBuilder final
    {
    public:
        ~BlueprintBuilder() = default;

        /// @brief Gets an entity of the spawned blueprint.
        ///
        /// Aborts if the name does not match any entity of the blueprint.
        ///
        /// @param name Entity name.
        /// @return Entity identifier.
        Entity entity(const std::string& name) const;

        /// @brief Gets a reference to a component of an entity of the spawned blueprint.
        ///
        /// Aborts if @p name does not match any entity of the blueprint.
        ///
        /// @tparam ComponentType Component type.
        /// @param name Entity name.
        /// @return Reference to the component.
        template <typename ComponentType>
        ComponentType& get(const std::string& name);

        /// @brief Adds components to the blueprint.
        ///
        /// Aborts if @p name does not match any entity of the blueprint.
        ///
        /// @tparam ComponentTypes Component types.
        /// @param name Entity name.
        /// @param components Components to add.
        /// @return Reference to this builder, for chaining.
        template <typename... ComponentTypes>
        BlueprintBuilder& add(const std::string& name, ComponentTypes&&... components);

    private:
        friend CommandBuffer;

        data::old::SerializationMap<Entity, std::string, EntityHash>
            mMap;                 ///< Maps entity names to the instantiated entities.
        CommandBuffer& mCommands; ///< Commands object that created this entity.

        /// @brief Constructs.
        /// @param map Map of entity names to the instantiated entities.
        /// @param commands Commands object that created this entity.
        BlueprintBuilder(data::old::SerializationMap<Entity, std::string, EntityHash>&& map, CommandBuffer& commands);
    };

    /// @brief System argument used to write ECS commands and execute them at a later time.
    ///
    /// Internally wraps a reference to a CommandBuffer object.
    ///
    /// @ingroup core-ecs-system
    class Commands final
    {
    public:
        /// @brief Constructs.
        /// @param buffer Command buffer to write to.
        Commands(CommandBuffer& buffer);

        /// @brief Move constructor.
        Commands(Commands&&) = default;

        /// @brief Adds components to an entity.
        /// @tparam ComponentTypes Component types.
        /// @param entity Entity identifier.
        /// @param components Components to add.
        template <typename... ComponentTypes>
        void add(Entity entity, ComponentTypes&&... components);

        /// @brief Removes components from an entity.
        /// @tparam ComponentTypes Component types.
        /// @param entity Entity identifier.
        template <typename... ComponentTypes>
        void remove(Entity entity);

        /// @brief Creates a new entity with the given components.
        /// @tparam ComponentTypes Component types.
        /// @param components Components to create with.
        /// @return Entity identifier.
        template <typename... ComponentTypes>
        EntityBuilder create(ComponentTypes&&... components);

        /// @brief Destroys an entity.
        /// @param entity Entity identifier.
        void destroy(Entity entity);

        /// @brief Spawns a blueprint into the world.
        /// @param blueprint Blueprint to spawn.
        /// @return Blueprint builder.
        BlueprintBuilder spawn(const Blueprint& blueprint);

    private:
        CommandBuffer& mBuffer; ///< Command buffer to write to.
    };

    /// @brief Stores commands to execute them later.
    /// @ingroup core-ecs-system
    class CommandBuffer final
    {
    public:
        /// @brief Constructs.
        /// @param world World to which the commands will be applied.
        CommandBuffer(World& world);
        ~CommandBuffer();

        /// @brief Adds components to an entity.
        /// @tparam ComponentTypes Component types.
        /// @param entity Entity identifier.
        /// @param components Components to add.
        template <typename... ComponentTypes>
        void add(Entity entity, ComponentTypes&&... components);

        /// @brief Removes components from an entity.
        /// @tparam ComponentTypes Component types.
        /// @param entity Entity identifier.
        template <typename... ComponentTypes>
        void remove(Entity entity);

        /// @brief Creates a new entity with the given components.
        /// @tparam ComponentTypes Component types.
        /// @param components Components to create with.
        /// @return Entity identifier.
        template <typename... ComponentTypes>
        EntityBuilder create(ComponentTypes&&... components);

        /// @brief Destroys an entity.
        /// @param entity Entity identifier.
        void destroy(Entity entity);

        /// @brief Spawns a blueprint into the world.
        /// @param blueprint Blueprint to spawn.
        /// @return Blueprint builder.
        BlueprintBuilder spawn(const Blueprint& blueprint);

        /// @brief Aborts the commands, rolling back any changes made.
        void abort();

        /// @brief Commits the commands to the world.
        void commit();

    private:
        friend EntityBuilder;
        friend BlueprintBuilder;
        friend Dispatcher;

        /// @brief Stores components of a specific type, queued for addition to the component
        /// manager.
        struct IBuffer
        {
            virtual ~IBuffer() = default;

            /// @brief Clears every component in the buffer.
            virtual void clear() = 0;

            /// @brief Moves the specified component from the buffer to the component manager. If
            /// the component doesn't exist, nothing happens.
            /// @param entity Entity identifier.
            /// @param manager Component manager.
            virtual void move(Entity entity, ComponentManager& manager) = 0;
        };

        /// @brief Implementation of the above interface for a component type
        /// @p ComponentType.
        /// @tparam ComponentType Component type.
        template <typename ComponentType>
        struct Buffer : IBuffer
        {
            // Interface methods implementation.

            void clear() override;
            void move(Entity entity, ComponentManager& manager) override;

            std::unordered_map<Entity, ComponentType, EntityHash> components; ///< Components in the buffer.
        };

        /// @brief Clears the commands.
        void clear();

        std::mutex mMutex; ///< Make this thread-safe.
        World& mWorld;     ///< World to which the commands will be applied.

        std::unordered_map<std::type_index, IBuffer*> mBuffers;        ///< Component buffers per component type.
        std::unordered_set<Entity, EntityHash> mCreated;               ///< Uncommitted created entities.
        std::unordered_set<Entity, EntityHash> mDestroyed;             ///< Uncommitted destroyed entities.
        std::unordered_map<Entity, Entity::Mask, EntityHash> mAdded;   ///< Mask of the uncommitted added components.
        std::unordered_map<Entity, Entity::Mask, EntityHash> mRemoved; ///< Mask of the uncommitted removed components.
        std::unordered_set<Entity, EntityHash> mChanged;               ///< Entities whose mask has changed.
    };

    // Implementation.

    template <typename ComponentType>
    ComponentType& EntityBuilder::get()
    {
        auto it = mCommands.mBuffers.find(typeid(ComponentType));
        if (it != mCommands.mBuffers.end())
        {
            auto buf = static_cast<CommandBuffer::Buffer<ComponentType>*>(it->second);
            auto it = buf->components.find(mEntity);
            if (it != buf->components.end())
            {
                return it->second;
            }
        }

        CUBOS_CRITICAL("Entity does not have the requested component");
        abort();
    }

    template <typename... ComponentTypes>
    EntityBuilder& EntityBuilder::add(ComponentTypes&&... components)
    {
        mCommands.add(mEntity, std::move(components)...);
        return *this;
    }

    template <typename ComponentType>
    ComponentType& BlueprintBuilder::get(const std::string& name)
    {
        auto it = mCommands.mBuffers.find(typeid(ComponentType));
        if (it != mCommands.mBuffers.end())
        {
            auto buf = static_cast<CommandBuffer::Buffer<ComponentType>*>(it->second);
            auto it = buf->components.find(this->entity(name));
            if (it != buf->components.end())
            {
                return it->second;
            }
        }

        CUBOS_CRITICAL("Entity does not have the requested component");
        abort();
    }

    template <typename... ComponentTypes>
    BlueprintBuilder& BlueprintBuilder::add(const std::string& name, ComponentTypes&&... components)
    {
        mCommands.add(this->entity(name), std::move(components)...);
        return *this;
    }

    template <typename... ComponentTypes>
    void Commands::add(Entity entity, ComponentTypes&&... components)
    {
        mBuffer.add(entity, std::move(components)...);
    }

    template <typename... ComponentTypes>
    void Commands::remove(Entity entity)
    {
        mBuffer.remove<ComponentTypes...>(entity);
    }

    template <typename... ComponentTypes>
    EntityBuilder Commands::create(ComponentTypes&&... components)
    {
        return mBuffer.create(std::move(components)...);
    }

    template <typename... ComponentTypes>
    void CommandBuffer::add(Entity entity, ComponentTypes&&... components)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        Entity::Mask& mask = mAdded[entity];
        mChanged.insert(entity);

        (
            [&]() {
                auto it = mBuffers.find(typeid(ComponentTypes));
                if (it == mBuffers.end())
                {
                    it = mBuffers.emplace(typeid(ComponentTypes), new Buffer<ComponentTypes>()).first;
                }

                std::size_t componentID = mWorld.mComponentManager.getID<ComponentTypes>();
                mask.set(componentID);
                static_cast<Buffer<ComponentTypes>*>(it->second)->components.erase(entity);
                static_cast<Buffer<ComponentTypes>*>(it->second)->components.emplace(entity, std::move(components));
            }(),
            ...);
    }

    template <typename... ComponentTypes>
    void CommandBuffer::remove(Entity entity)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        Entity::Mask& mask = mRemoved[entity];
        mChanged.insert(entity);

        (
            [&]() {
                std::size_t componentID = mWorld.mComponentManager.getID<ComponentTypes>();
                mask.set(componentID);
            }(),
            ...);
    }

    template <typename... ComponentTypes>
    EntityBuilder CommandBuffer::create(ComponentTypes&&... components)
    {
        std::lock_guard<std::mutex> lock(mMutex);

        Entity entity = mWorld.mEntityManager.create(0);
        mCreated.insert(entity);
        mChanged.insert(entity);
        Entity::Mask& mask = mAdded[entity];

        (
            [&]() {
                auto it = mBuffers.find(typeid(ComponentTypes));
                if (it == mBuffers.end())
                {
                    it = mBuffers.emplace(typeid(ComponentTypes), new Buffer<ComponentTypes>()).first;
                }

                std::size_t componentID = mWorld.mComponentManager.getID<ComponentTypes>();
                mask.set(componentID);
                static_cast<Buffer<ComponentTypes>*>(it->second)->components.erase(entity);
                static_cast<Buffer<ComponentTypes>*>(it->second)->components.emplace(entity, components);
            }(),
            ...);

        return {entity, *this};
    }

    template <typename ComponentType>
    void CommandBuffer::Buffer<ComponentType>::clear()
    {
        this->components.clear();
    }

    template <typename ComponentType>
    void CommandBuffer::Buffer<ComponentType>::move(Entity entity, ComponentManager& manager)
    {
        auto it = this->components.find(entity);
        if (it != this->components.end())
        {
            manager.add(entity.index, std::move(it->second));
            this->components.erase(it);
        }
    }
} // namespace cubos::core::ecs
