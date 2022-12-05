#include <cubos/core/ecs/commands.hpp>
#include <cubos/core/ecs/blueprint.hpp>

using namespace cubos::core::ecs;

EntityBuilder::EntityBuilder(Entity entity, Commands& commands) : eEntity(entity), commands(commands)
{
    // Do nothing.
}

Entity EntityBuilder::entity() const
{
    return this->eEntity;
}

BlueprintBuilder::BlueprintBuilder(data::SerializationMap<Entity, std::string>&& map, Commands& commands)
    : map(std::move(map)), commands(commands)
{
    // Do nothing.
}

Entity BlueprintBuilder::entity(const std::string& name) const
{
    if (!this->map.hasId(name))
    {
        CUBOS_CRITICAL("No entity with name '{}'", name);
        abort();
    }

    return this->map.getRef(name);
}

Commands::Commands(World& world) : world(world)
{
    // Do nothing.
}

Commands::Commands(Commands&& other) : world(other.world)
{
    this->buffers = std::move(other.buffers);
    this->created = std::move(other.created);
    this->destroyed = std::move(other.destroyed);
    this->added = std::move(other.added);
    this->removed = std::move(other.removed);
    this->changed = std::move(other.changed);
}

Commands::~Commands()
{
    this->abort();
}

void Commands::destroy(Entity entity)
{
    std::lock_guard<std::mutex> lock(this->mutex);

    this->destroyed.insert(entity);
}

#include <iostream>

BlueprintBuilder Commands::spawn(const Blueprint& blueprint)
{
    data::SerializationMap<Entity, std::string> map;
    for (uint32_t i = 0; i < static_cast<uint32_t>(blueprint.map.size()); ++i)
    {
        map.add(this->create().entity(), blueprint.map.getId(Entity(i, 0)));
    }

    auto desHandleCtx = [&](data::Deserializer& des, data::Handle& handle) {
        uint64_t id;
        des.read(id);
        handle = blueprint.handles.at(id);
    };

    for (auto& buf : blueprint.buffers)
    {
        buf.second->addAll(*this, map, desHandleCtx);
    }

    return BlueprintBuilder(std::move(map), *this);
}

void Commands::commit()
{
    std::lock_guard<std::mutex> lock(this->mutex);

    // 1. Components are removed.
    for (auto& [entity, removed] : this->removed)
    {
        for (size_t componentId = 1; componentId <= CUBOS_CORE_ECS_MAX_COMPONENTS; ++componentId)
        {
            if (removed.test(componentId))
            {
                this->world.componentManager.remove(entity.index, componentId);
            }
        }
    }

    // 2. Entities are destroyed.
    for (auto entity : this->destroyed)
    {
        this->world.componentManager.removeAll(entity.index);
        this->world.entityManager.destroy(entity);
    }

    // 3. Components are added.
    for (auto& [entity, added] : this->added)
    {
        for (auto& buf : this->buffers)
        {
            buf.second->move(entity, this->world.componentManager);
        }
    }

    // 4. Entities masks are set.
    for (auto& entity : this->changed)
    {
        // Get the old mask.
        auto mask = this->world.entityManager.getMask(entity);

        // Remove the components.
        auto it = this->removed.find(entity);
        if (it != this->removed.end())
        {
            mask &= ~it->second;
        }

        // Add the new components.
        it = this->added.find(entity);
        if (it != this->added.end())
        {
            mask |= it->second;
        }

        // If its a new entity, set the activation bit.
        if (this->created.find(entity) != this->created.end())
        {
            mask.set(0);
        }

        // Update the mask.
        this->world.entityManager.setMask(entity, mask);
    }

    this->clear();
}

void Commands::abort()
{
    std::lock_guard<std::mutex> lock(this->mutex);

    for (auto entity : this->created)
    {
        this->world.entityManager.destroy(entity);
    }

    this->clear();
}

void Commands::clear()
{
    for (auto& buffer : this->buffers)
    {
        delete buffer.second;
    }

    this->buffers.clear();
    this->created.clear();
    this->destroyed.clear();
    this->added.clear();
    this->removed.clear();
    this->changed.clear();
}
