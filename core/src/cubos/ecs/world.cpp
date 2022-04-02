#include <cubos/ecs/world.hpp>

using namespace cubos::ecs;

World::~World()
{
    for (auto* storage : storages)
    {
        delete storage;
    }
}

size_t World::create()
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

    return id;
}
