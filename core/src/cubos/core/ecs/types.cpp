#include <cubos/core/ecs/types.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::ecs::DataTypeId;
using cubos::core::ecs::Types;
using cubos::core::reflection::Type;
using cubos::core::reflection::TypeRegistry;

const DataTypeId DataTypeId::Invalid = {.inner = UINT32_MAX};

void Types::addComponent(const reflection::Type& type)
{
    this->add(type, Kind::Component);
}

void Types::addRelation(const reflection::Type& type)
{
    this->add(type, Kind::Relation);
}

void Types::add(const reflection::Type& type, Kind kind)
{
    CUBOS_ASSERT(!mTypes.contains(type), "Type {} already registered", type.name());

    auto id = DataTypeId{.inner = static_cast<uint32_t>(mEntries.size())};
    auto inserted = mNames.emplace(type.name(), id).second;
    CUBOS_ASSERT(inserted, "A different type with the same name {} was already registered", type.name());
    mTypes.insert(type, id);
    mEntries.push_back({.type = &type, .kind = kind});
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
    return mEntries[id.inner].kind == Kind::Component;
}

bool Types::isRelation(DataTypeId id) const
{
    return mEntries[id.inner].kind == Kind::Relation;
}

TypeRegistry Types::components() const
{
    TypeRegistry registry{};
    for (const auto& entry : mEntries)
    {
        if (entry.kind == Kind::Component)
        {
            registry.insert(*entry.type);
        }
    }
    return registry;
}
