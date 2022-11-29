#ifndef CUBOS_CORE_ECS_COMMANDS_HPP
#define CUBOS_CORE_ECS_COMMANDS_HPP

#include <cubos/core/ecs/world.hpp>

#include <unordered_map>
#include <unordered_set>
#include <mutex>

namespace cubos::core::ecs
{
    class Blueprint;
    class Commands;
    class Dispatcher;

    /// Used to edit an entity created by the Commands object.
    class EntityBuilder final
    {
    public:
        ~EntityBuilder() = default;

        /// Gets the entity this builder is editing.
        /// @returns The entity.
        Entity entity() const;

        /// Gets a reference to a component of the entity.
        /// @tparam ComponentType The type of the component.
        /// @returns A reference to the component.
        template <typename ComponentType> ComponentType& get();

        /// Adds component to the entity.
        /// @tparam ComponentTypes The types of the components.
        /// @param components The components to add.
        template <typename... ComponentTypes> EntityBuilder& add(ComponentTypes&&... components);

    private:
        friend Commands;

        /// @param entity The entity being edited.
        /// @param commands The commands object that created this entity.
        EntityBuilder(Entity entity, Commands& commands);

        Entity eEntity;     ///< The entity being edited.
        Commands& commands; ///< The commands object that created this entity.
    };

    /// Used to edit an instance of a blueprint spawned by the Commands object.
    class BlueprintBuilder final
    {
    public:
        ~BlueprintBuilder() = default;

        /// Gets the entity of the instance this builder is editing.
        /// Aborts if the name is not found.
        /// @param name The name of the entity.
        /// @returns The entity.
        Entity entity(const std::string& name) const;

        /// Gets a reference to a component of the blueprint.
        /// Aborts if the name is not found.
        /// @tparam ComponentType The type of the component.
        /// @returns A reference to the component.
        template <typename ComponentType> ComponentType& get(const std::string& name);

        /// Adds component to the blueprint.
        /// Aborts if the name is not found.
        /// @tparam ComponentTypes The types of the components.
        /// @param components The components to add.
        template <typename... ComponentTypes>
        BlueprintBuilder& add(const std::string& name, ComponentTypes&&... components);

    private:
        friend Commands;

        data::SerializationMap<Entity, std::string> map; ///< Maps entity names to the instantiated entities.
        Commands& commands;                              ///< The commands object that created this entity.

        /// @param map The map of entity names to the instantiated entities.
        /// @param commands The commands object that created this entity.
        BlueprintBuilder(data::SerializationMap<Entity, std::string>&& map, Commands& commands);
    };

    /// Object responsible for storing ECS commands to execute them later.
    class Commands final
    {
    public:
        /// TODO: make this private after implementing the Cubos class.
        /// @param world The world to which the commands will be applied.
        Commands(World& world);
        Commands(Commands&&);
        ~Commands();

        /// Adds components to an entity.
        /// @tparam ComponentTypes The types of the components to be added.
        /// @param entity The entity to which the components will be added.
        /// @param components The components to add.
        template <typename... ComponentTypes> void add(Entity entity, ComponentTypes&&... components);

        /// Removes components from an entity.
        /// @tparam ComponentTypes The types of the components to be removed.
        /// @param entity The entity from which the components will be removed.
        template <typename... ComponentTypes> void remove(Entity entity);

        /// Creates a new entity with the given components.
        /// @tparam ComponentTypes The types of the components to be added.
        /// @param components The components to add.
        /// @returns The new entity.
        template <typename... ComponentTypes> EntityBuilder create(ComponentTypes&&... components);

        /// Destroys an entity.
        /// @param entity The entity to destroy.
        void destroy(Entity entity);

        /// Spawns an instance of a blueprint into the world.
        /// @param blueprint The blueprint to spawn.
        /// @returns Blueprint builder which allows components to be overridden.
        BlueprintBuilder spawn(const Blueprint& blueprint);

    private:
        friend EntityBuilder;
        friend Dispatcher;

        /// Object used internally which stores components of a specific type, queued for addition to the component
        /// manager.
        struct IBuffer
        {
            /// Clears every component in the buffer.
            virtual void clear() = 0;

            /// Moves the specified component from the buffer to the component manager.
            /// If the component doesn't exist, nothing happens.
            /// @param entity The entity whose component will be moved.
            /// @param manager The component manager to which the component will be moved.
            virtual void move(Entity entity, ComponentManager& manager) = 0;
        };

        /// Implementation of the IBuffer interface for a specific component type.
        /// @tparam ComponentType The type of the component.
        template <typename ComponentType> struct Buffer : IBuffer
        {
            // Interface methods implementation.

            virtual void clear() override;
            virtual void move(Entity entity, ComponentManager& manager) override;

            std::unordered_map<Entity, ComponentType> components; ///< The components in the buffer.
        };

        /// Aborts the commands, rolling back any changes made.
        void abort();

        /// Clears the commands.
        void clear();

        /// Commits the commands to the world.
        void commit();

        std::mutex mutex; ///< Make this thread-safe.
        World& world;     ///< The world to which the commands will be applied.

        std::unordered_map<std::type_index, IBuffer*> buffers; ///< Component buffers per component type.
        std::unordered_set<Entity> created;                    ///< The uncommitted created entities.
        std::unordered_set<Entity> destroyed;                  ///< The uncommitted destroyed entities.
        std::unordered_map<Entity, Entity::Mask> added;        ///< The mask of the uncommitted added components.
        std::unordered_map<Entity, Entity::Mask> removed;      ///< The mask of the uncommitted removed components.
        std::unordered_set<Entity> changed;                    ///< Entities whose mask has changed.
    };

    // Implementation.

    template <typename ComponentType> ComponentType& EntityBuilder::get()
    {
        auto it = this->commands.buffers.find(typeid(ComponentType));
        if (it != this->commands.buffers.end())
        {
            auto buf = static_cast<Commands::Buffer<ComponentType>*>(it->second);
            auto it = buf->components.find(this->eEntity);
            if (it != buf->components.end())
            {
                return it->second;
            }
        }

        CUBOS_CRITICAL("Entity does not have the requested component");
        abort();
    }

    template <typename... ComponentTypes> EntityBuilder& EntityBuilder::add(ComponentTypes&&... components)
    {
        this->commands.add(this->eEntity, std::move(components)...);
        return *this;
    }

    template <typename ComponentType> ComponentType& BlueprintBuilder::get(const std::string& name)
    {
        auto it = this->commands.buffers.find(typeid(ComponentType));
        if (it != this->commands.buffers.end())
        {
            auto buf = static_cast<Commands::Buffer<ComponentType>*>(it->second);
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
        this->commands.add(this->entity(name), std::move(components)...);
        return *this;
    }

    template <typename... ComponentTypes> void Commands::add(Entity entity, ComponentTypes&&... components)
    {
        std::lock_guard<std::mutex> lock(this->mutex);
        Entity::Mask& mask = this->added[entity];
        this->changed.insert(entity);

        (
            [&]() {
                auto it = this->buffers.find(typeid(ComponentTypes));
                if (it == this->buffers.end())
                {
                    it = this->buffers.emplace(typeid(ComponentTypes), new Buffer<ComponentTypes>()).first;
                }

                size_t componentID = this->world.componentManager.getID<ComponentTypes>();
                mask.set(componentID);
                static_cast<Buffer<ComponentTypes>*>(it->second)->components.erase(entity);
                static_cast<Buffer<ComponentTypes>*>(it->second)->components.emplace(entity, std::move(components));
            }(),
            ...);
    }

    template <typename... ComponentTypes> void Commands::remove(Entity entity)
    {
        std::lock_guard<std::mutex> lock(this->mutex);
        Entity::Mask& mask = this->removed[entity];
        this->changed.insert(entity);

        (
            [&]() {
                size_t componentID = this->world.componentManager.getID<ComponentTypes>();
                mask.set(componentID);
            }(),
            ...);
    }

    template <typename... ComponentTypes> EntityBuilder Commands::create(ComponentTypes&&... components)
    {
        std::lock_guard<std::mutex> lock(this->mutex);

        Entity entity = this->world.entityManager.create(0);
        this->created.insert(entity);
        this->changed.insert(entity);
        Entity::Mask& mask = this->added[entity];

        (
            [&]() {
                auto it = this->buffers.find(typeid(ComponentTypes));
                if (it == this->buffers.end())
                {
                    it = this->buffers.emplace(typeid(ComponentTypes), new Buffer<ComponentTypes>()).first;
                }

                size_t componentID = this->world.componentManager.getID<ComponentTypes>();
                mask.set(componentID);
                static_cast<Buffer<ComponentTypes>*>(it->second)->components.erase(entity);
                static_cast<Buffer<ComponentTypes>*>(it->second)->components.emplace(entity, components);
            }(),
            ...);

        return EntityBuilder(entity, *this);
    }

    template <typename ComponentType> void Commands::Buffer<ComponentType>::clear()
    {
        this->components.clear();
    }

    template <typename ComponentType>
    void Commands::Buffer<ComponentType>::move(Entity entity, ComponentManager& manager)
    {
        auto it = this->components.find(entity);
        if (it != this->components.end())
        {
            manager.add(entity.index, std::move(it->second));
            this->components.erase(it);
        }
    }
} // namespace cubos::core::ecs

#endif // CUBOS_CORE_ECS_COMMANDS_HPP
