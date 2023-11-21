#include <cubos/core/ecs/component/manager.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::reflection::Type;
using cubos::core::reflection::TypeRegistry;

using namespace cubos::core::ecs;

void ComponentManager::registerType(const Type& type)
{
    if (!mTypeToIds.contains(type))
    {
        mTypeToIds.insert(type, static_cast<uint32_t>(mStorages.size()) + 1); // Component ids start at 1.
        mStorages.emplace_back(type);
    }
}

TypeRegistry ComponentManager::registry() const
{
    TypeRegistry registry{};
    for (const auto& [type, _] : mTypeToIds)
    {
        registry.insert(*type);
    }
    return registry;
}

uint32_t ComponentManager::id(const Type& type) const
{
    CUBOS_ASSERT(mTypeToIds.contains(type), "Component type {} was not registered", type.name());
    return mTypeToIds.at(type);
}

const Type& ComponentManager::type(uint32_t id) const
{
    return mStorages[static_cast<std::size_t>(id) - 1].type();
}

void ComponentManager::insert(uint32_t index, uint32_t id, void* value)
{
    mStorages[static_cast<std::size_t>(id) - 1].insert(index, value);
}

void ComponentManager::erase(uint32_t index, uint32_t id)
{
    mStorages[static_cast<std::size_t>(id) - 1].erase(index);
}

void ComponentManager::erase(uint32_t index)
{
    for (auto& storage : mStorages)
    {
        storage.erase(index);
    }
}

Storage& ComponentManager::storage(uint32_t id)
{
    return mStorages[static_cast<std::size_t>(id) - 1];
}

const Storage& ComponentManager::storage(uint32_t id) const
{
    return mStorages[static_cast<std::size_t>(id) - 1];
}
