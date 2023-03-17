#include <cubos/core/ecs/world.hpp>
#include <cubos/core/ecs/registry.hpp>

using namespace cubos::core;
using namespace cubos::core::ecs;

static const auto directMap = data::SerializationMap<Entity, std::string>(
    [](const Entity& entity, std::string& name) {
        name = std::to_string(entity.index) + "#" + std::to_string(entity.generation);
        return true;
    },
    [](Entity& entity, const std::string& name) {
        auto pos = name.find('#');
        if (pos != std::string::npos)
        {
            entity.index = std::stoul(name.substr(0, pos));
            entity.generation = std::stoul(name.substr(pos + 1));
            return true;
        }
        return false;
    });

World::World(size_t initialCapacity) : entityManager(initialCapacity)
{
    // Do nothing.
    // Edu: BAH!
}

void World::destroy(Entity entity)
{
    this->entityManager.destroy(entity);
    this->componentManager.removeAll(entity.index);
    CUBOS_DEBUG("Destroyed entity {}", entity.index);
}

data::Package World::pack(Entity entity, data::Context& context) const
{
    Entity::Mask mask = this->entityManager.getMask(entity);

    auto pkg = data::Package(data::Package::Type::Object);
    for (size_t i = 1; i < mask.size(); ++i)
    {
        if (mask.test(i))
        {
            auto name = Registry::name(this->componentManager.getType(i));
            pkg.fields().push_back({name, this->componentManager.pack(entity.index, i, context)});
        }
    }

    return pkg;
}

bool World::unpack(Entity entity, const data::Package& package, data::Context& context)
{
    if (package.type() != data::Package::Type::Object)
    {
        CUBOS_ERROR("Entities must be packaged as objects");
        return false;
    }

    Entity::Mask mask = this->entityManager.getMask(entity);
    bool success = true;

    for (const auto& field : package.fields())
    {
        auto index = Registry::index(field.first);
        if (index == typeid(void))
        {
            CUBOS_ERROR("Unknown component type '{}'", field.first);
            success = false;
            continue;
        }

        auto id = this->componentManager.getIDFromIndex(index);
        if (this->componentManager.unpack(entity.index, id, field.second, context))
        {
            mask.set(id);
        }
        else
        {
            CUBOS_ERROR("Could not unpack component '{}'", field.first);
            success = false;
        }
    }

    this->entityManager.setMask(entity, mask);
    return success;
}

World::Iterator World::begin() const
{
    return this->entityManager.begin();
}

World::Iterator World::end() const
{
    return this->entityManager.end();
}
