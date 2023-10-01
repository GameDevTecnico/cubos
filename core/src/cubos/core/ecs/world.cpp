#include <cubos/core/ecs/component/registry.hpp>
#include <cubos/core/ecs/world.hpp>

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
        auto type = Registry::type(field.first);
        if (!type.has_value())
        {
            CUBOS_ERROR("Unknown component type '{}'", field.first);
            success = false;
            continue;
        }

        auto id = mComponentManager.getIDFromIndex(*type);
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
