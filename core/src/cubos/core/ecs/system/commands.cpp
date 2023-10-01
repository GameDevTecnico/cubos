#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/system/commands.hpp>

using namespace cubos::core::ecs;

EntityBuilder::EntityBuilder(Entity entity, CommandBuffer& commands)
    : mEntity(entity)
    , mCommands(commands)
{
    // Do nothing.
}

Entity EntityBuilder::entity() const
{
    return mEntity;
}

BlueprintBuilder::BlueprintBuilder(data::old::SerializationMap<Entity, std::string, EntityHash>&& map,
                                   CommandBuffer& commands)
    : mMap(std::move(map))
    , mCommands(commands)
{
    // Do nothing.
}

Entity BlueprintBuilder::entity(const std::string& name) const
{
    if (!mMap.hasId(name))
    {
        CUBOS_CRITICAL("No entity with name '{}'", name);
        abort();
    }

    return mMap.getRef(name);
}

Commands::Commands(CommandBuffer& buffer)
    : mBuffer(buffer)
{
    // Do nothing.
}

void Commands::destroy(Entity entity)
{
    mBuffer.destroy(entity);
}

BlueprintBuilder Commands::spawn(const Blueprint& blueprint)
{
    return mBuffer.spawn(blueprint);
}

CommandBuffer::CommandBuffer(World& world)
    : mWorld(world)
{
    // Do nothing.
}

CommandBuffer::~CommandBuffer()
{
    this->abort();
}

void CommandBuffer::destroy(Entity entity)
{
    std::lock_guard<std::mutex> lock(mMutex);

    mDestroyed.insert(entity);
}

BlueprintBuilder CommandBuffer::spawn(const Blueprint& blueprint)
{
    data::old::SerializationMap<Entity, std::string, EntityHash> map;
    for (uint32_t i = 0; i < static_cast<uint32_t>(blueprint.mMap.size()); ++i)
    {
        map.add(this->create().entity(), blueprint.mMap.getId(Entity(i, 0)));
    }

    data::old::Context context;
    context.push(map);
    for (const auto& buf : blueprint.mBuffers)
    {
        buf.second->addAll(*this, context);
    }

    return {std::move(map), *this};
}

void CommandBuffer::commit()
{
    std::lock_guard<std::mutex> lock(mMutex);

    // 1. Components are removed.
    for (auto& [entity, removed] : mRemoved)
    {
        for (std::size_t componentId = 1; componentId <= CUBOS_CORE_ECS_MAX_COMPONENTS; ++componentId)
        {
            if (removed.test(componentId))
            {
                mWorld.mComponentManager.remove(entity.index, componentId);
            }
        }
    }

    // 2. Entities are destroyed.
    for (auto entity : mDestroyed)
    {
        mWorld.mComponentManager.removeAll(entity.index);
        mWorld.mEntityManager.destroy(entity);
    }

    // 3. Components are added.
    for (auto& [entity, added] : mAdded)
    {
        for (auto& buf : mBuffers)
        {
            buf.second->move(entity, mWorld.mComponentManager);
        }
    }

    // 4. Entities masks are set.
    for (const auto& entity : mChanged)
    {
        // Get the old mask.
        auto mask = mWorld.mEntityManager.getMask(entity);

        // Remove the components.
        auto it = mRemoved.find(entity);
        if (it != mRemoved.end())
        {
            mask &= ~it->second;
        }

        // Add the new components.
        it = mAdded.find(entity);
        if (it != mAdded.end())
        {
            mask |= it->second;
        }

        // If its a new entity, set the activation bit.
        if (mCreated.find(entity) != mCreated.end())
        {
            mask.set(0);
        }

        // Update the mask.
        mWorld.mEntityManager.setMask(entity, mask);
    }

    this->clear();
}

void CommandBuffer::abort()
{
    std::lock_guard<std::mutex> lock(mMutex);

    for (auto entity : mCreated)
    {
        mWorld.mEntityManager.destroy(entity);
    }

    this->clear();
}

void CommandBuffer::clear()
{
    for (auto& buffer : mBuffers)
    {
        delete buffer.second;
    }

    mBuffers.clear();
    mCreated.clear();
    mDestroyed.clear();
    mAdded.clear();
    mRemoved.clear();
    mChanged.clear();
}
