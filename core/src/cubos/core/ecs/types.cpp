#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/ecs/types.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::ecs::DataTypeId;
using cubos::core::ecs::DataTypeIdHash;
using cubos::core::ecs::Types;
using cubos::core::reflection::Type;
using cubos::core::reflection::TypeRegistry;

const DataTypeId DataTypeId::Invalid = {.inner = UINT32_MAX};

std::size_t DataTypeIdHash::operator()(const DataTypeId& id) const
{
    return id.inner;
}

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
    mEntries.push_back({.type = &type, .kind = kind, .isSymmetric = type.has<SymmetricTrait>()});
}

DataTypeId Types::id(const reflection::Type& type) const
{
    CUBOS_ASSERT(mTypes.contains(type), "Type {} not registered", type.name());
    return mTypes.at(type);
}

DataTypeId Types::id(const std::string& name) const
{
    CUBOS_ASSERT(mNames.contains(name), "Type {} not registered", name);
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

bool Types::isSymmetricRelation(DataTypeId id) const
{
    return mEntries[id.inner].kind == Kind::Relation && mEntries[id.inner].isSymmetric;
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
