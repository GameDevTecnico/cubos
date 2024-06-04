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

void Types::reset()
{
    mTypes.clear();
    mNames.clear();
    mEntries.clear();
}

void Types::addResource(const reflection::Type& type)
{
    this->add(type, Kind::Resource);
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
    mEntries.push_back({
        .type = &type,
        .kind = kind,
        .isSymmetric = type.has<SymmetricTrait>(),
        .isTree = type.has<TreeTrait>(),
    });

    CUBOS_ASSERT(!mEntries.back().isTree || !mEntries.back().isSymmetric, "Symmetric tree relations are not supported");
}

void Types::remove(const reflection::Type& type)
{
    CUBOS_ASSERT(mTypes.contains(type), "Type {} not registered", type.name());

    auto id = mTypes.at(type);
    mTypes.erase(type);
    mNames.erase(type.name());
    mEntries[id.inner].type = nullptr;
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

bool Types::contains(const Type& type) const
{
    return mTypes.contains(type);
}

bool Types::contains(const std::string& name) const
{
    return mNames.contains(name);
}

bool Types::isResource(DataTypeId id) const
{
    return mEntries[id.inner].kind == Kind::Resource;
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

bool Types::isTreeRelation(DataTypeId id) const
{
    return mEntries[id.inner].kind == Kind::Relation && mEntries[id.inner].isTree;
}

TypeRegistry Types::resources() const
{
    TypeRegistry registry{};
    for (const auto& entry : mEntries)
    {
        if (entry.kind == Kind::Resource && entry.type != nullptr)
        {
            registry.insert(*entry.type);
        }
    }
    return registry;
}

TypeRegistry Types::components() const
{
    TypeRegistry registry{};
    for (const auto& entry : mEntries)
    {
        if (entry.kind == Kind::Component && entry.type != nullptr)
        {
            registry.insert(*entry.type);
        }
    }
    return registry;
}

TypeRegistry Types::relations() const
{
    TypeRegistry registry{};
    for (const auto& entry : mEntries)
    {
        if (entry.kind == Kind::Relation && entry.type != nullptr)
        {
            registry.insert(*entry.type);
        }
    }
    return registry;
}
