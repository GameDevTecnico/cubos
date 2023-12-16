#include <cubos/core/ecs/types.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::ecs::DataTypeId;
using cubos::core::ecs::Types;
using cubos::core::reflection::Type;
using cubos::core::reflection::TypeRegistry;

const DataTypeId DataTypeId::Invalid = {.inner = UINT32_MAX};

void Types::addComponent(const reflection::Type& type)
{
    auto id = DataTypeId{.inner = static_cast<uint32_t>(mEntries.size())};
    mTypes.insert(type, id);
    mNames.emplace(type.name(), id);
    mEntries.push_back({.type = &type, .isComponent = true});
}

DataTypeId Types::id(const reflection::Type& type) const
{
    return mTypes.at(type);
}

DataTypeId Types::id(const std::string& name) const
{
    return mNames.at(name);
}

const Type& Types::type(DataTypeId id) const
{
    return *mEntries[id.inner].type;
}

bool Types::contains(const std::string& name) const
{
    return mNames.contains(name);
}

bool Types::isComponent(DataTypeId id) const
{
    return mEntries[id.inner].isComponent;
}

TypeRegistry Types::components() const
{
    TypeRegistry registry{};
    for (const auto& entry : mEntries)
    {
        registry.insert(*entry.type);
    }
    return registry;
}
