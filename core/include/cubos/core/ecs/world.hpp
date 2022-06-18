#ifndef CUBOS_ECS_WORLD_HPP
#define CUBOS_ECS_WORLD_HPP

#include <cubos/core/log.hpp>
#include <cubos/core/ecs/storage.hpp>

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
        ~World();

        /// @brief Creates a new entity.
        /// @tparam ComponentTypes The types of the components to be added when the entity is created.
        /// @param components The initial values for the components.
        template <typename... ComponentTypes> uint64_t create(ComponentTypes... components);

        /// @brief Removes an entity.
        /// @param entity Entity ID.
        void remove(uint64_t entity);

        /// @brief Registers a component type.
        /// @tparam T Component type.
        /// @param storage Storage for the component type.
        template <typename T> void registerComponent();

        /// @brief Adds a component to an entity.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        /// @param value Initial value of the component
        template <typename T> void addComponent(uint64_t entity, T value = {});

        /// @brief Adds components to an entity.
        /// @tparam ComponentTypes The types of the components to be added.
        /// @param components The initial values for the components.
        template <typename... ComponentTypes> void addComponents(uint64_t entity, ComponentTypes... components);

        /// @brief Gets a reference to a component of an entity.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        template <typename T> T& getComponent(uint64_t entity);

        /// @brief Checks if an entity has a component.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        template <typename T> bool hasComponent(uint64_t entity);

        /// @brief Removes a component from an entity.
        /// @tparam T Component type to be removed.
        /// @param entity Entity ID.
        template <typename T> void removeComponent(uint64_t entity);

        /// @brief Removes components from an entity.
        /// @tparam ComponentTypes Component types to be removed.
        /// @param entity Entity ID.
        template <typename... ComponentTypes> void removeComponents(uint64_t entity);

    private:
        template <typename... ComponentTypes> friend class WorldView;

        /// Keeps track of the next available global component ID.
        static size_t nextGlobalComponentId;

        /// Maps global component IDs to local component IDs.
        std::unordered_map<size_t, size_t> globalToLocalComponentIds;

        std::vector<std::uint32_t> entityData;
        std::vector<std::uint32_t> availableEntities;
        std::vector<IStorage*> storages;

        size_t nextEntityId = 0;
        size_t elementsPerEntity;

        /// Utility function which returns the global ID of a component.
        /// @tparam T Component type.
        /// @return Global ID of the component.
        template <typename T> static size_t getGlobalComponentId();

        /// Utility function which returns the local ID of a component in this world.
        /// @tparam T Component type.
        template <typename T> size_t getLocalComponentID();
    };

    // Implementation.

    template <typename... ComponentTypes> uint64_t World::create(ComponentTypes... components)
    {
        if (entityData.size() == 0)
            elementsPerEntity = 1 + (31 + storages.size()) / 32;

        uint64_t id;
        if (!availableEntities.empty())
        {
            uint32_t index = availableEntities.back();
            uint32_t version = entityData[elementsPerEntity * index];
            id = ((uint64_t)version << 32) | index;
        }
        else
        {
            id = nextEntityId++;
            for (size_t i = 0; i < elementsPerEntity; i++)
            {
                entityData.push_back(0);
            }
        }

        addComponents(id, components...);

        return id;
    }

    template <typename T> void World::registerComponent()
    {
        static_assert(std::is_same<T, typename T::Storage::Type>(),
                      "A component can't use a storage for a different component type!");
        assert(entityData.size() == 0);
        auto globalId = World::getGlobalComponentId<T>();
        if (this->globalToLocalComponentIds.find(globalId) == this->globalToLocalComponentIds.end())
        {
            this->globalToLocalComponentIds[globalId] = storages.size();
            storages.push_back(new typename T::Storage());
        }
    }

    template <typename T> void World::addComponent(uint64_t entity, T value)
    {
        uint32_t entityIndex = (uint32_t)entity;
        if (entity >> 32 != entityData[entityIndex * elementsPerEntity])
        {
            logError("World::addComponent() failed because entity {} was already removed!", entity);
            return;
        }

        size_t componentId = this->getLocalComponentID<T>();
        Storage<T>* storage = (Storage<T>*)storages[componentId];
        // Set the entity mask for this component
        entityData[entityIndex * elementsPerEntity + 1 + componentId / 32] |= 1 << (componentId % 32);

        storage->insert(entityIndex, value);
    }

    template <typename... ComponentTypes> void World::addComponents(uint64_t entity, ComponentTypes... components)
    {
        ([&](auto& component) { addComponent(entity, component); }(components), ...);
    }

    template <typename T> T& World::getComponent(uint64_t entity)
    {
        uint32_t entityIndex = (uint32_t)entity;
        if (entity >> 32 != entityData[entityIndex * elementsPerEntity])
        {
            logCritical("World::getComponent() failed because entity {} was already removed!", entity);
            abort();
        }

        size_t componentId = this->getLocalComponentID<T>();
        if ((entityData[entityIndex * elementsPerEntity + 1 + componentId / 32] & (1 << (componentId % 32))) == 0)
        {
            logCritical("World::getComponent() failed because entity {} doesn't have component {}!", entity,
                        typeid(T).name());
            abort();
        }

        Storage<T>* storage = (Storage<T>*)storages[componentId];
        return *storage->get(entityIndex);
    }

    template <typename T> bool World::hasComponent(uint64_t entity)
    {
        uint32_t entityIndex = (uint32_t)entity;
        if (entity >> 32 != entityData[entityIndex * elementsPerEntity])
        {
            logCritical("World::hasComponent() failed because entity {} was already removed!", entity);
            abort();
        }

        size_t componentId = this->getLocalComponentID<T>();
        return (entityData[entityIndex * elementsPerEntity + 1 + componentId / 32] & (1 << (componentId % 32))) != 0;
    }

    template <typename T> void World::removeComponent(uint64_t entity)
    {
        uint32_t entityIndex = (uint32_t)entity;
        if (entity >> 32 != entityData[entityIndex * elementsPerEntity])
        {
            logError("World::getComponent() failed because entity {} was already removed!", entity);
            return;
        }

        size_t componentId = this->getLocalComponentID<T>();
        ((Storage<T>*)storages[componentId])->erase(entityIndex);
        entityData[entityIndex * elementsPerEntity + 1 + componentId / 32] ^= 1 << (componentId % 32);
    }

    template <typename... ComponentTypes> void World::removeComponents(uint64_t entity)
    {
        ([&]() { removeComponent<ComponentTypes>(entity); }(), ...);
    }

    template <typename T> size_t World::getGlobalComponentId()
    {
        static size_t id = World::nextGlobalComponentId++;
        return id;
    }

    template <typename T> size_t World::getLocalComponentID()
    {
        auto globalId = World::getGlobalComponentId<T>();
        return this->globalToLocalComponentIds.at(globalId);
    }
} // namespace cubos::core::ecs

#endif // CUBOS_ECS_WORLD_HPP
