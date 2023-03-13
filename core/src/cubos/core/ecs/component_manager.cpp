#include <cubos/core/ecs/component_manager.hpp>
#include <cubos/core/ecs/registry.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

std::optional<std::string_view> cubos::core::ecs::getComponentName(std::type_index type)
{
    return Registry::name(type);
}

void ComponentManager::registerComponent(std::type_index type)
{
    if (this->typeToIds.find(type) == this->typeToIds.end())
    {
        auto storage = Registry::createStorage(type);
        if (storage == nullptr)
        {
            CUBOS_CRITICAL("Component type '{}' is not registered in the global registry", type.name());
            abort();
        }

        this->typeToIds[type] = this->entries.size() + 1; // Component ids start at 1.
        this->entries.emplace_back(std::move(storage));
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
    this->entries[componentId - 1].storage->erase(id);
}

void ComponentManager::removeAll(uint32_t id)
{
    for (auto& entry : this->entries)
    {
        entry.storage->erase(id);
    }
}

ComponentManager::Entry::Entry(std::unique_ptr<IStorage> storage) : storage(std::move(storage))
{
    this->mutex = std::make_unique<std::shared_mutex>();
}

data::Package ComponentManager::pack(uint32_t id, size_t componentId, data::Context& context) const
{
    return this->entries[componentId - 1].storage->pack(id, context);
}

bool ComponentManager::unpack(uint32_t id, size_t componentId, const data::Package& package, data::Context& context)
{
    return this->entries[componentId - 1].storage->unpack(id, package, context);
}
