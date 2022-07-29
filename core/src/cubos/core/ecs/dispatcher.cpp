#include <cubos/core/ecs/dispatcher.hpp>

using namespace cubos::core::ecs;

void Dispatcher::callSystems(World& world)
{
    for (auto& system : systems)
    {
        system->call(world);
    }
}