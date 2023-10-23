#include <cubos/core/ecs/component/registry.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

World::World(std::size_t initialCapacity)
    : mEntityManager(initialCapacity)
{
    // Do nothing.
    // Edu: BAH!
}

void World::destroy(Entity entity)
{
    mEntityManager.destroy(entity);
    mComponentManager.removeAll(entity.index);
    CUBOS_DEBUG("Destroyed entity {}", entity.index);
}

bool World::isAlive(Entity entity) const
{
    return mEntityManager.isAlive(entity);
}

World::Components World::components(Entity entity)
{
    return Components{*this, entity};
}

World::ConstComponents World::components(Entity entity) const
{
    return ConstComponents{*this, entity};
}

data::old::Package World::pack(Entity entity, data::old::Context* context) const
{
    CUBOS_ASSERT(this->isAlive(entity), "Entity is not alive");

    Entity::Mask mask = mEntityManager.getMask(entity);

    auto pkg = data::old::Package(data::old::Package::Type::Object);
    for (std::size_t i = 1; i < mask.size(); ++i)
    {
        if (mask.test(i))
        {
            auto name = Registry::name(mComponentManager.getType(i));
            pkg.fields().push_back({std::string(name.value()), mComponentManager.pack(entity.index, i, context)});
        }
    }

    return pkg;
}

bool World::unpack(Entity entity, const data::old::Package& package, data::old::Context* context)
{
    if (package.type() != data::old::Package::Type::Object)
    {
        CUBOS_ERROR("Entities must be packaged as objects");
        return false;
    }

    // Remove all existing components.
    mComponentManager.removeAll(entity.index);

    Entity::Mask mask{1};
    bool success = true;

    for (const auto& field : package.fields())
    {
        const auto* type = Registry::type(field.first);
        if (type == nullptr)
        {
            CUBOS_ERROR("Unknown component type '{}'", field.first);
            success = false;
            continue;
        }

        auto id = mComponentManager.getID(*type);
        if (mComponentManager.unpack(entity.index, id, field.second, context))
        {
            mask.set(id);
        }
        else
        {
            CUBOS_ERROR("Could not unpack component '{}'", field.first);
            success = false;
        }
    }

    mEntityManager.setMask(entity, mask);
    return success;
}

World::Iterator World::begin() const
{
    return mEntityManager.begin();
}

World::Iterator World::end() const
{
    return mEntityManager.end();
}

World::Components::Components(World& world, Entity entity)
    : mWorld{world}
    , mEntity{entity}
{
    CUBOS_ASSERT(world.isAlive(entity));
}

bool World::Components::has(const reflection::Type& type) const
{
    std::size_t componentId = mWorld.mComponentManager.getID(type);
    return mWorld.mEntityManager.getMask(mEntity).test(componentId);
}

void* World::Components::get(const reflection::Type& type)
{
    CUBOS_ASSERT(this->has(type));
    std::size_t componentId = mWorld.mComponentManager.getID(type);
    return mWorld.mComponentManager.storage(componentId)->get(mEntity.index);
}

auto World::Components::begin() -> Iterator
{
    return Iterator{*this, false};
}

auto World::Components::end() -> Iterator
{
    return Iterator{*this, true};
}

World::ConstComponents::ConstComponents(const World& world, Entity entity)
    : mWorld{world}
    , mEntity{entity}
{
    CUBOS_ASSERT(world.isAlive(entity));
}

bool World::ConstComponents::has(const reflection::Type& type) const
{
    std::size_t componentId = mWorld.mComponentManager.getID(type);
    return mWorld.mEntityManager.getMask(mEntity).test(componentId);
}

const void* World::ConstComponents::get(const reflection::Type& type) const
{
    CUBOS_ASSERT(this->has(type));
    std::size_t componentId = mWorld.mComponentManager.getID(type);
    return mWorld.mComponentManager.storage(componentId)->get(mEntity.index);
}

auto World::ConstComponents::begin() const -> Iterator
{
    return Iterator{*this, false};
}

auto World::ConstComponents::end() const -> Iterator
{
    return Iterator{*this, true};
}

World::Components::Iterator::Iterator(Components& components, bool end)
    : mComponents{components}
{
    const auto& mask = mComponents.mWorld.mEntityManager.getMask(mComponents.mEntity);

    if (end)
    {
        mId = mask.size();
    }

    while (mId < mask.size() && !mask.test(mId))
    {
        mId += 1;
    }
}

bool World::Components::Iterator::operator==(const Iterator& other) const
{
    return mId == other.mId && mComponents.mEntity == other.mComponents.mEntity;
}

auto World::Components::Iterator::operator*() const -> const Component&
{
    const auto& mask = mComponents.mWorld.mEntityManager.getMask(mComponents.mEntity);
    CUBOS_ASSERT(mId < mask.size(), "Iterator is out of bounds");
    mComponent.type = &mComponents.mWorld.mComponentManager.getType(mId);
    mComponent.value = mComponents.get(*mComponent.type);
    return mComponent;
}

auto World::Components::Iterator::operator->() const -> const Component*
{
    return &this->operator*();
}

auto World::Components::Iterator::operator++() -> Iterator&
{
    const auto& mask = mComponents.mWorld.mEntityManager.getMask(mComponents.mEntity);
    CUBOS_ASSERT(mId < mask.size(), "Iterator is out of bounds");
    do
    {
        mId += 1;
    } while (mId < mask.size() && !mask.test(mId));

    return *this;
}

World::ConstComponents::Iterator::Iterator(const ConstComponents& components, bool end)
    : mComponents{components}
{
    const auto& mask = mComponents.mWorld.mEntityManager.getMask(mComponents.mEntity);

    if (end)
    {
        mId = mask.size();
    }

    while (mId < mask.size() && !mask.test(mId))
    {
        mId += 1;
    }
}

bool World::ConstComponents::Iterator::operator==(const Iterator& other) const
{
    return mId == other.mId && mComponents.mEntity == other.mComponents.mEntity;
}

auto World::ConstComponents::Iterator::operator*() const -> const Component&
{
    const auto& mask = mComponents.mWorld.mEntityManager.getMask(mComponents.mEntity);
    CUBOS_ASSERT(mId < mask.size(), "Iterator is out of bounds");
    mComponent.type = &mComponents.mWorld.mComponentManager.getType(mId);
    mComponent.value = mComponents.get(*mComponent.type);
    return mComponent;
}

auto World::ConstComponents::Iterator::operator->() const -> const Component*
{
    return &this->operator*();
}

auto World::ConstComponents::Iterator::operator++() -> Iterator&
{
    const auto& mask = mComponents.mWorld.mEntityManager.getMask(mComponents.mEntity);
    CUBOS_ASSERT(mId < mask.size(), "Iterator is out of bounds");
    do
    {
        mId += 1;
    } while (mId < mask.size() && !mask.test(mId));

    return *this;
}
