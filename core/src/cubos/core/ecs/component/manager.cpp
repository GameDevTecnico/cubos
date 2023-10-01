#include <cubos/core/ecs/component/manager.hpp>
#include <cubos/core/ecs/component/registry.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

std::optional<std::string_view> cubos::core::ecs::getComponentName(std::type_index type)
{
    return Registry::name(type);
}

void ComponentManager::registerComponent(std::type_index type)
{
    if (mTypeToIds.find(type) == mTypeToIds.end())
    {
        auto storage = Registry::createStorage(type);
        if (storage == nullptr)
        {
            CUBOS_CRITICAL("Component type '{}' is not registered in the global registry", type.name());
            abort();
        }

        mTypeToIds[type] = mEntries.size() + 1; // Component ids start at 1.
        mEntries.emplace_back(std::move(storage));
    }
}

std::size_t ComponentManager::getIDFromIndex(std::type_index type) const
{
    if (auto it = mTypeToIds.find(type); it != mTypeToIds.end())
    {
        return it->second;
    }

    CUBOS_CRITICAL("Component type '{}' is not registered in the component manager", type.name());
    abort();
}

std::type_index ComponentManager::getType(std::size_t id) const
{
    for (const auto& pair : mTypeToIds)
    {
        if (pair.second == id)
        {
            return pair.first;
        }
    }

    CUBOS_CRITICAL("No component found with ID {}", id);
    abort();
}

void ComponentManager::remove(uint32_t id, std::size_t componentId)
{
    mEntries[componentId - 1].storage->erase(id);
}

void ComponentManager::removeAll(uint32_t id)
{
    for (auto& entry : mEntries)
    {
        entry.storage->erase(id);
    }
}

ComponentManager::Entry::Entry(std::unique_ptr<IStorage> storage)
    : storage(std::move(storage))
{
    this->mutex = std::make_unique<std::shared_mutex>();
}

data::old::Package ComponentManager::pack(uint32_t id, std::size_t componentId, data::old::Context* context) const
{
    return mEntries[componentId - 1].storage->pack(id, context);
}

bool ComponentManager::unpack(uint32_t id, std::size_t componentId, const data::old::Package& package,
                              data::old::Context* context)
{
    return mEntries[componentId - 1].storage->unpack(id, package, context);
}
