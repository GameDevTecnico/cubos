#ifndef CUBOS_ECS_WORLD_HPP
#define CUBOS_ECS_WORLD_HPP

#include <cassert>
#include <cinttypes>

#include <cubos/ecs/storage.hpp>

namespace cubos::ecs
{
    class World
    {
    private:
        std::vector<std::uint8_t> masks;
        std::vector<IStorage*> storages;

        size_t nextEntityId = 0;
        size_t bytesPerMask;

        template <typename T> size_t getComponentID();

    public:
        ~World();

        /// @brief Creates a new entity.
        /// @tparam ComponentTypes The types of the components to be added when the entity is created.
        /// @param components The initial values for the components.
        template <typename... ComponentTypes> size_t create(ComponentTypes... components);

        /// @brief Register a component type.
        /// @tparam T Component type.
        /// @param storage Storage for the component type.
        template <typename T> size_t registerComponent();

        /// @brief Add a component to an entity.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        /// @param value Initial value of the component
        template <typename T> T* addComponent(size_t entity, T value = {});

        /// @brief Adds components to an entity.
        /// @tparam ComponentTypes The types of the components to be added.
        /// @param components The initial values for the components.
        template <typename... ComponentTypes> void addComponents(size_t entity, ComponentTypes... components);

        /// @brief Get a component of an entity.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        template <typename T> T* getComponent(size_t entity);

        template <typename... ComponentTypes> friend class WorldView;
    };

    template <typename... ComponentTypes> struct WorldView
    {
        World* world;
        std::vector<uint8_t> mask;

        WorldView(World& w);

        struct Iterator
        {
            World* world;
            size_t current;
            std::vector<uint8_t>* mask;

            Iterator(World* w, size_t index, std::vector<uint8_t>* m);

            size_t operator*() const;

            bool operator==(const Iterator& other) const;

            bool operator!=(const Iterator& other) const;

            bool isValidID();

            Iterator& operator++();
        };

        bool isValidIndex(size_t index);

        Iterator begin();

        Iterator end();
    };

    template <typename T> size_t World::getComponentID()
    {
        static size_t id = storages.size();
        return id;
    }

    template <typename... ComponentTypes> size_t World::create(ComponentTypes... components)
    {
        if (masks.size() == 0)
        {
            bytesPerMask = (7 + storages.size()) / 8;
        }

        size_t id = nextEntityId++;
        for (size_t i = 0; i < bytesPerMask; i++)
        {
            uint8_t n = 0;
            masks.push_back(n);
        }

        addComponents(id, components...);

        return id;
    }

    template <typename T> size_t World::registerComponent()
    {
        assert(_masks.size() == 0);
        size_t component_id = getComponentID<T>();
        static_assert(std::is_same<T, typename T::Storage::Type>(),
                      "A component can't use a storage for a different component type!");
        storages.push_back(new typename T::Storage());
        return component_id;
    }

    template <typename T> T* World::addComponent(size_t entity, T value)
    {
        size_t componentId = getComponentID<T>();
        Storage<T>* storage = (Storage<T>*)storages[componentId];
        // Set the entity mask for this component
        masks[entity * bytesPerMask + componentId / 8] |= 1 << (componentId % 8);

        return storage->insert(entity, value);
    }

    template <typename... ComponentTypes> void World::addComponents(size_t entity, ComponentTypes... components)
    {
        ([&](auto& component) { addComponent(entity, component); }(components), ...);
    }

    template <typename T> T* World::getComponent(size_t entity)
    {
        size_t componentId = getComponentID<T>();

        if ((masks[entity * bytesPerMask + componentId / 8] & (1 << (componentId % 8))) == 0)
            return nullptr;

        Storage<T>* storage = (Storage<T>*)storages[componentId];
        return storage->get(entity);
    }

    template <typename... ComponentTypes> WorldView<ComponentTypes...>::WorldView(World& w) : world(&w)
    {
        mask.resize(world->bytesPerMask);
        size_t componentIds[] = {world->getComponentID<ComponentTypes>()...};
        for (auto id : componentIds)
        {
            mask[id / 8] |= 1 << (id % 8);
        }
    }

    template <typename... ComponentTypes>
    WorldView<ComponentTypes...>::Iterator::Iterator(World* w, size_t index, std::vector<uint8_t>* m)
        : world(w), current(index), mask(m)
    {
    }

    template <typename... ComponentTypes> size_t WorldView<ComponentTypes...>::Iterator::operator*() const
    {
        return current;
    }

    template <typename... ComponentTypes>
    bool WorldView<ComponentTypes...>::Iterator::operator==(const Iterator& other) const
    {
        return current == other.current || current == world->nextEntityId;
    }

    template <typename... ComponentTypes>
    bool WorldView<ComponentTypes...>::Iterator::operator!=(const Iterator& other) const
    {
        return current != other.current && current != world->nextEntityId;
    }

    template <typename... ComponentTypes> bool WorldView<ComponentTypes...>::Iterator::isValidID()
    {
        for (size_t i = 0; i < mask->size(); i++)
        {
            if ((mask->at(i) & world->masks[current * world->bytesPerMask + i]) != mask->at(i))
                return false;
        }
        return true;
    }

    template <typename... ComponentTypes>
    typename WorldView<ComponentTypes...>::Iterator& WorldView<ComponentTypes...>::Iterator::operator++()
    {
        do
        {
            current++;
        } while (current < world->nextEntityId && !isValidID());
        return *this;
    }

    template <typename... ComponentTypes> bool WorldView<ComponentTypes...>::isValidIndex(size_t index)
    {
        for (size_t i = 0; i < mask.size(); i++)
        {
            if ((mask[i] & world->masks[index * world->bytesPerMask + i]) != mask[i])
                return false;
        }

        return true;
    }

    template <typename... ComponentTypes>
    typename WorldView<ComponentTypes...>::Iterator WorldView<ComponentTypes...>::begin()
    {
        size_t firstIndex = 0;
        while (firstIndex < world->nextEntityId && !isValidIndex(firstIndex))
        {
            firstIndex++;
        }

        Iterator it(world, firstIndex, &mask);
        return it;
    }

    template <typename... ComponentTypes>
    typename WorldView<ComponentTypes...>::Iterator WorldView<ComponentTypes...>::end()
    {
        Iterator it(world, world->nextEntityId, &mask);
        return it;
    }
} // namespace cubos::ecs

#endif // CUBOS_ECS_WORLD_HPP
