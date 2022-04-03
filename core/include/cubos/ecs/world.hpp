#ifndef CUBOS_ECS_WORLD_HPP
#define CUBOS_ECS_WORLD_HPP

#include <cassert>
#include <cinttypes>

#include "storage.hpp"

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

        /// Creates a new entity.
        size_t create();

        /// \brief Register a component type.
        /// @tparam T Component type.
        /// @param storage Storage for the component type.
        template <typename T> size_t registerComponent(Storage<T>* storage);

        /// \brief Add a component to an entity.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        /// @param value Initial value of the component
        template <typename T> T* addComponent(size_t entity, T value);

        /// \brief Get a component of an entity.
        /// @tparam T Component type.
        /// @param entity Entity ID.
        template <typename T> T* getComponent(size_t entity);

        template <typename... ComponentTypes> friend class WorldView;
    };

    template <typename... ComponentTypes> struct WorldView
    {
        World* world;
        std::vector<uint8_t> mask;

        WorldView(World& w) : world(&w)
        {
            mask.resize(world->bytesPerMask);
            size_t componentIds[] = {world->getComponentID<ComponentTypes>()...};
            for (auto id : componentIds)
            {
                mask[id / 8] |= 1 << (id % 8);
            }
        }

        struct Iterator
        {
            World* world;
            size_t current;
            std::vector<uint8_t>* mask;

            Iterator(World* w, size_t index, std::vector<uint8_t>* m) : world(w), current(index), mask(m)
            {
            }

            size_t operator*() const
            {
                return current;
            }

            bool operator==(const Iterator& other) const
            {
                return current == other.current || current == world->nextEntityId;
            }

            bool operator!=(const Iterator& other) const
            {
                return current != other.current && current != world->nextEntityId;
            }

            bool isValidID()
            {
                for (size_t i = 0; i < mask->size(); i++)
                {
                    if ((mask->at(i) & world->masks[current * world->bytesPerMask + i]) != mask->at(i))
                        return false;
                }
                return true;
            }

            Iterator& operator++()
            {
                do
                {
                    current++;
                } while (current < world->nextEntityId && !isValidID());
                return *this;
            }
        };

        bool isValidIndex(size_t index)
        {
            for (size_t i = 0; i < mask.size(); i++)
            {
                if ((mask[i] & world->masks[index * world->bytesPerMask + i]) != mask[i])
                    return false;
            }

            return true;
        }

        Iterator begin()
        {
            size_t firstIndex = 0;
            while (firstIndex < world->nextEntityId && !isValidIndex(firstIndex))
            {
                firstIndex++;
            }

            Iterator it(world, firstIndex, &mask);
            return it;
        }

        Iterator end()
        {
            Iterator it(world, world->nextEntityId, &mask);
            return it;
        }
    };

    template <typename T> size_t World::getComponentID()
    {
        static size_t id = storages.size();
        return id;
    }

    template <typename T> size_t World::registerComponent(Storage<T>* storage)
    {
        assert(_masks.size() == 0);
        size_t component_id = getComponentID<T>();
        storages.push_back(storage);
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

    template <typename T> T* World::getComponent(size_t entity)
    {
        size_t componentId = getComponentID<T>();

        if ((masks[entity * bytesPerMask + componentId / 8] & (1 << (componentId % 8))) == 0)
            return nullptr;

        Storage<T>* storage = (Storage<T>*)storages[componentId];
        return storage->get(entity);
    }
} // namespace cubos::ecs

#endif // CUBOS_ECS_WORLD_HPP
