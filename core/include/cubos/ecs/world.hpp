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
        std::vector<std::uint8_t> _masks;
        std::vector<IStorage*> _storages;

        size_t _next_entity_id = 0;
        size_t _bytes_per_mask;

        template <typename T> size_t getComponentID()
        {
            static size_t id = _storages.size();
            return id;
        }

    public:
        ~World()
        {
            for (auto* storage : _storages)
            {
                delete storage;
            }
        }

        size_t create()
        {
            if (_masks.size() == 0)
            {
                _bytes_per_mask = (7 + _storages.size()) / 8;
            }

            size_t id = _next_entity_id++;
            for (size_t i = 0; i < _bytes_per_mask; i++)
            {
                uint8_t n = 0;
                _masks.push_back(n);
            }

            return id;
        }

        template <typename T> size_t registerComponent(Storage<T>* storage);

        template <typename T> T* addComponent(size_t entity, T value);

        template <typename T> T* getComponent(size_t entity);

        template <typename... ComponentTypes> friend class WorldView;
    };

    template <typename... ComponentTypes> struct WorldView
    {
        World* _world;
        std::vector<uint8_t> _mask;

        WorldView(World& world) : _world(&world)
        {
            _mask.resize(_world->_bytes_per_mask);
            size_t componentIds[] = {_world->getComponentID<ComponentTypes>()...};
            for (auto id : componentIds)
            {
                _mask[id / 8] |= 1 << (id % 8);
            }
        }

        struct Iterator
        {
            World* _world;
            size_t current;
            std::vector<uint8_t>* _mask;

            Iterator(World* world, size_t index, std::vector<uint8_t>* mask)
                : _world(world), current(index), _mask(mask)
            {
            }

            size_t operator*() const
            {
                return current;
            }

            bool operator==(const Iterator& other) const
            {
                return current == other.current || current == _world->_next_entity_id;
            }

            bool operator!=(const Iterator& other) const
            {
                return current != other.current && current != _world->_next_entity_id;
            }

            bool isValidID()
            {
                for (size_t i = 0; i < _mask->size(); i++)
                {
                    if ((_mask->at(i) & _world->_masks[current * _world->_bytes_per_mask + i]) != _mask->at(i))
                        return false;
                }
                return true;
            }

            Iterator& operator++()
            {
                do
                {
                    current++;
                } while (current < _world->_next_entity_id && !isValidID());
                return *this;
            }
        };

        bool isValidIndex(size_t index)
        {
            for (size_t i = 0; i < _mask.size(); i++)
            {
                if ((_mask[i] & _world->_masks[index * _world->_bytes_per_mask + i]) != _mask[i])
                    return false;
            }

            return true;
        }

        Iterator begin()
        {
            size_t firstIndex = 0;
            while (firstIndex < _world->_next_entity_id && !isValidIndex(firstIndex))
            {
                firstIndex++;
            }

            Iterator it(_world, firstIndex, &_mask);
            return it;
        }

        Iterator end()
        {
            Iterator it(_world, _world->_next_entity_id, &_mask);
            return it;
        }
    };

    template <typename T> size_t World::registerComponent(Storage<T>* storage)
    {
        assert(_masks.size() == 0);
        size_t component_id = getComponentID<T>();
        _storages.push_back(storage);
        return component_id;
    }

    template <typename T> T* World::addComponent(size_t entity, T value)
    {
        size_t component_id = getComponentID<T>();
        Storage<T>* storage = (Storage<T>*)_storages[component_id];
        // Set the entity mask for this component
        _masks[entity * _bytes_per_mask + component_id / 8] |= 1 << (component_id % 8);

        return storage->insert(entity, value);
    }

    template <typename T> T* World::getComponent(size_t entity)
    {
        size_t component_id = getComponentID<T>();

        if ((_masks[entity * _bytes_per_mask + component_id / 8] & (1 << (component_id % 8))) == 0)
            return nullptr;

        Storage<T>* storage = (Storage<T>*)_storages[component_id];
        return storage->get(entity);
    }
} // namespace cubos::ecs

#endif // CUBOS_ECS_WORLD_HPP
