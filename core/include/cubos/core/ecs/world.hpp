#ifndef CUBOS_ECS_WORLD_HPP
#define CUBOS_ECS_WORLD_HPP

#include <cubos/core/log.hpp>
#include <cubos/core/ecs/storage.hpp>
#include <cubos/core/ecs/resource_manager.hpp>
#include <cubos/core/ecs/entity_manager.hpp>

#include <cassert>
#include <unordered_map>
#include <cstddef>

namespace cubos::core::ecs
{

    /// @brief World is used as a container for all of the entity and component data.
    /// Components are stored in abstract containers called storages.
    /// @see Storage
    class World
    {
    public:
        /// @param initialCapacity The initial capacity of the world.
        World(size_t initialCapacity = 1024);
        ~World();

        /// Registers a new resource type.
        /// Unsafe to call during any reads or writes, should be called at the start of the program.
        /// @tparam T The type of the resource.
        /// @tparam TArgs The types of the arguments of the constructor of the resource.
        /// @param args The arguments of the constructor of the resource.
        template <typename T, typename... TArgs> void registerResource(TArgs... args);

        /// Reads a resource, locking it for reading.
        /// @tparam T The type of the resource.
        /// @returns A lock referring to the resource.
        template <typename T> ReadResource<T> readResource() const;

        /// Writes a resource, locking it for writing.
        /// @tparam T The type of the resource.
        /// @returns A lock referring to the resource.
        template <typename T> WriteResource<T> writeResource() const;

        /// @brief Creates a new entity.
        /// @tparam ComponentTypes The types of the components to be added when the entity is created.
        /// @param components The initial values for the components.
        template <typename... ComponentTypes> Entity create(ComponentTypes... components);

        /// @brief Removes an entity.
        /// @param entity Entity ID.
        void remove(Entity entity);

        /// @brief Registers a component type.
        /// @tparam T Component type.
        /// @param storage Storage for the component type.
        template <typename T> void registerComponent();

        /// @brief Adds a component to an entity.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        /// @param value Initial value of the component
        template <typename T> void addComponent(Entity entity, T value = {});

        /// @brief Adds components to an entity.
        /// @tparam ComponentTypes The types of the components to be added.
        /// @param components The initial values for the components.
        template <typename... ComponentTypes> void addComponents(Entity entity, ComponentTypes... components);

        /// @brief Gets a reference to a component of an entity.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        template <typename T> T& getComponent(Entity entity);

        /// @brief Checks if an entity has a component.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        template <typename T> bool hasComponent(Entity entity);

        /// @brief Removes a component from an entity.
        /// @tparam T Component type to be removed.
        /// @param entity Entity ID.
        template <typename T> void removeComponent(Entity entity);

        /// @brief Removes components from an entity.
        /// @tparam ComponentTypes Component types to be removed.
        /// @param entity Entity ID.
        template <typename... ComponentTypes> void removeComponents(Entity entity);

    private:
        template <typename... ComponentTypes> friend class Query;

        /// Keeps track of the next available global component ID.
        static size_t nextGlobalComponentId;

        /// Maps global component IDs to local component IDs.
        std::unordered_map<size_t, size_t> globalToLocalComponentIds;

        std::vector<IStorage*> storages;

        ResourceManager resourceManager;
        EntityManager entityManager;

        /// Utility function which returns the global ID of a component.
        /// @tparam T Component type.
        /// @return Global ID of the component.
        template <typename T> static size_t getGlobalComponentId();

        /// Utility function which returns the local ID of a component in this world.
        /// @tparam T Component type.
        template <typename T> size_t getLocalComponentID() const;
    };

    // Implementation.

    template <typename T, typename... TArgs> void World::registerResource(TArgs... args)
    {
        this->resourceManager.add<T>(args...);
    }

    template <typename T> ReadResource<T> World::readResource() const
    {
        return this->resourceManager.read<T>();
    }

    template <typename T> WriteResource<T> World::writeResource() const
    {
        return this->resourceManager.write<T>();
    }

    template <typename... ComponentTypes> Entity World::create(ComponentTypes... components)
    {
        auto entity = this->entityManager.create(0); // Ricardo: ew
        this->addComponents(entity, components...);  // Edu: BAH!
        // this->componentManager.add<ComponentTypes...>(id, components...);
        return entity;
    }

    template <typename T> void World::registerComponent()
    {
        static_assert(std::is_same<T, typename T::Storage::Type>(),
                      "A component can't use a storage for a different component type!");
        auto globalId = World::getGlobalComponentId<T>();
        if (this->globalToLocalComponentIds.find(globalId) == this->globalToLocalComponentIds.end())
        {
            this->globalToLocalComponentIds[globalId] = storages.size();
            storages.push_back(new typename T::Storage());
        }
    }

    template <typename T> void World::addComponent(Entity entity, T value)
    {
        if (!this->entityManager.isValid(entity))
        {
            logError("World::addComponent() failed because entity {} isn't valid anymore!", entity.index);
            return;
        }

        size_t componentId = this->getLocalComponentID<T>();
        Storage<T>* storage = (Storage<T>*)this->storages[componentId];
        // Set the entity mask for this component
        auto mask = this->entityManager.getMask(entity);
        mask.set(componentId);
        this->entityManager.setMask(entity, mask);
        storage->insert(entity.index, value);
    }

    template <typename... ComponentTypes> void World::addComponents(Entity entity, ComponentTypes... components)
    {
        ([&](auto& component) { this->addComponent(entity, component); }(components), ...);
    }

    template <typename T> T& World::getComponent(Entity entity)
    {
        if (!this->entityManager.isValid(entity))
        {
            logCritical("World::getComponent() failed because entity {} isn't valid anymore!", entity.index);
            abort();
        }

        size_t componentId = this->getLocalComponentID<T>();
        if (!this->entityManager.getMask(entity).test(componentId))
        {
            logCritical("World::getComponent() failed because entity {} doesn't have component {}!", entity.index,
                        typeid(T).name());
            abort();
        }

        Storage<T>* storage = (Storage<T>*)storages[componentId];
        return *storage->get(entity.index);
    }

    template <typename T> bool World::hasComponent(Entity entity)
    {
        if (!this->entityManager.isValid(entity))
        {
            logCritical("World::hasComponent() failed because entity {} was already removed!", entity.index);
            abort();
        }

        size_t componentId = this->getLocalComponentID<T>();
        return this->entityManager.getMask(entity).test(componentId);
    }

    template <typename T> void World::removeComponent(Entity entity)
    {
        if (!this->entityManager.isValid(entity))
        {
            logError("World::getComponent() failed because entity {} was already removed!", entity.index);
            return;
        }

        size_t componentId = this->getLocalComponentID<T>();
        ((Storage<T>*)storages[componentId])->erase(entity.index);
        auto mask = this->entityManager.getMask(entity);
        mask.set(componentId, false);
        this->entityManager.setMask(entity, mask);
    }

    template <typename... ComponentTypes> void World::removeComponents(Entity entity)
    {
        ([&]() { this->removeComponent<ComponentTypes>(entity); }(), ...);
    }

    template <typename T> size_t World::getGlobalComponentId()
    {
        static size_t id = World::nextGlobalComponentId++;
        return id;
    }

    template <typename T> size_t World::getLocalComponentID() const
    {
        auto globalId = World::getGlobalComponentId<T>();
        return this->globalToLocalComponentIds.at(globalId);
    }
} // namespace cubos::core::ecs

#endif // CUBOS_ECS_WORLD_HPP
