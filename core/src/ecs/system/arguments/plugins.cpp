#include <cubos/core/ecs/system/arguments/plugins.hpp>
#include <cubos/core/log.hpp>

using cubos::core::ecs::Plugins;

Plugins::Plugins(PluginQueue* queue)
    : mQueue(queue)
{
    CUBOS_ASSERT(queue != nullptr, "Plugins system argument is not valid in this context");
}

void Plugins::add(Plugin plugin)
{
    std::lock_guard guard(mQueue->mutex);
    mQueue->toAdd.push_back(plugin);
}

void Plugins::remove(Plugin plugin)
{
    std::lock_guard guard(mQueue->mutex);
    mQueue->toRemove.push_back(plugin);
}
