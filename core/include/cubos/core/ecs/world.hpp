#ifndef CUBOS_ECS_WORLD_HPP
#define CUBOS_ECS_WORLD_HPP

#include <cassert>
#include <cstddef>

#include <cubos/core/ecs/storage.hpp>

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

        /// @brief Register a component type.
        /// @tparam T Component type.
        /// @param storage Storage for the component type.
        template <typename T> size_t registerComponent();

        /// @brief Add a component to an entity.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        /// @param value Initial value of the component
        template <typename T> T* addComponent(uint64_t entity, T value = {});

        /// @brief Adds components to an entity.
        /// @tparam ComponentTypes The types of the components to be added.
        /// @param components The initial values for the components.
        template <typename... ComponentTypes> void addComponents(uint64_t entity, ComponentTypes... components);

        /// @brief Get a component of an entity.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        template <typename T> T* getComponent(uint64_t entity);

        /// @brief Remove a component from an entity.
        /// @tparam T Component type to be removed.
        /// @param entity Entity ID.
        template <typename T> void removeComponent(uint64_t entity);

        /// @brief Remove components from an entity.
        /// @tparam ComponentTypes Component types to be removed.
        /// @param entity Entity ID.
        template <typename... ComponentTypes> void removeComponents(uint64_t entity);

        template <typename... ComponentTypes> friend class WorldView;

    private:
        std::vector<std::uint32_t> entityData;
        std::vector<std::uint32_t> availableEntities;
        std::vector<IStorage*> storages;

        size_t nextEntityId = 0;
        size_t elementsPerEntity;

        template <typename T> size_t getComponentID();
    };

    template <typename T> size_t World::getComponentID()
    {
        static size_t id = storages.size();
        return id;
    }

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

    template <typename T> size_t World::registerComponent()
    {
        assert(entityData.size() == 0);
        size_t component_id = getComponentID<T>();
        static_assert(std::is_same<T, typename T::Storage::Type>(),
                      "A component can't use a storage for a different component type!");
        storages.push_back(new typename T::Storage());
        return component_id;
    }

    template <typename T> T* World::addComponent(uint64_t entity, T value)
    {
        uint32_t entityIndex = (uint32_t)entity;
        if (entity >> 32 != entityData[entityIndex * elementsPerEntity])
            return nullptr;

        size_t componentId = getComponentID<T>();
        Storage<T>* storage = (Storage<T>*)storages[componentId];
        // Set the entity mask for this component
        entityData[entityIndex * elementsPerEntity + 1 + componentId / 8] |= 1 << (componentId % 8);

        return storage->insert(entityIndex, value);
    }

    template <typename... ComponentTypes> void World::addComponents(uint64_t entity, ComponentTypes... components)
    {
        ([&](auto& component) { addComponent(entity, component); }(components), ...);
    }

    template <typename T> T* World::getComponent(uint64_t entity)
    {
        uint32_t entityIndex = (uint32_t)entity;
        if (entity >> 32 != entityData[entityIndex * elementsPerEntity])
            return nullptr;

        size_t componentId = getComponentID<T>();
        if ((entityData[entityIndex * elementsPerEntity + 1 + componentId / 8] & (1 << (componentId % 8))) == 0)
            return nullptr;

        Storage<T>* storage = (Storage<T>*)storages[componentId];
        return storage->get(entityIndex);
    }

    template <typename T> void World::removeComponent(uint64_t entity)
    {
        uint32_t entityIndex = (uint32_t)entity;
        if (entity >> 32 != entityData[entityIndex * elementsPerEntity])
            return;

        size_t componentId = getComponentID<T>();
        ((Storage<T>*)storages[componentId])->erase(entityIndex);
        entityData[entityIndex * elementsPerEntity + 1 + componentId / 8] ^= 1 << (componentId % 8);
    }

    template <typename... ComponentTypes> void World::removeComponents(uint64_t entity)
    {
        ([&]() { removeComponent<ComponentTypes>(entity); }(), ...);
    }

} // namespace cubos::core::ecs

#endif // CUBOS_ECS_WORLD_HPP
