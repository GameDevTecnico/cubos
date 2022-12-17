#include <cubos/core/ecs/component_manager.hpp>
#include <cubos/core/ecs/registry.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

const std::string& cubos::core::ecs::getComponentName(std::type_index type)
{
    return Registry::name(type);
}

ComponentManager::~ComponentManager()
{
    for (auto& entry : this->entries)
    {
        delete entry.storage;
    }
}

size_t ComponentManager::getIDFromIndex(std::type_index type) const
{
    return this->typeToIds.at(type);
}

std::type_index ComponentManager::getType(size_t id) const
{
    for (const auto& pair : this->typeToIds)
    {
        if (pair.second == id)
        {
            return pair.first;
        }
    }

    CUBOS_CRITICAL("No component found with ID {}", id);
    abort();
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

data::Package ComponentManager::pack(uint32_t id, size_t componentId,
                                     const data::SerializationMap<Entity, std::string>& map,
                                     data::Handle::SerContext handleCtx) const
{
    return this->entries[componentId - 1].storage->pack(id, map, handleCtx);
}

bool ComponentManager::unpack(uint32_t id, size_t componentId, const data::Package& package,
                              const data::SerializationMap<Entity, std::string>& map,
                              data::Handle::DesContext handleCtx)
{
    return this->entries[componentId - 1].storage->unpack(id, package, map, handleCtx);
}
