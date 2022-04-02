#include <cubos/ecs/world.hpp>

using namespace cubos::ecs;

World::~World()
{
    for (auto* storage : _storages)
    {
        delete storage;
    }
}

size_t World::create()
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
