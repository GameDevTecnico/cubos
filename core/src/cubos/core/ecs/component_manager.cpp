#include <cubos/core/ecs/component_manager.hpp>

using namespace cubos::core::ecs;

ComponentManager::~ComponentManager()
{
    for (auto& entry : this->entries)
    {
        delete entry.storage;
    }
}

void ComponentManager::remove(uint32_t id, size_t componentId)
{
    this->entries[componentId - 1].storage->erase(componentId);
}

void ComponentManager::removeAll(uint32_t id)
{
    for (auto& entry : this->entries)
    {
        entry.storage->erase(id);
    }
}

ComponentManager::Entry::Entry(IStorage* storage) : storage(storage)
{
    this->mutex = std::make_unique<std::shared_mutex>();
}
