#include <cubos/core/ecs/observer/observers.hpp>

using cubos::core::ecs::ObserverId;
using cubos::core::ecs::Observers;

Observers::~Observers()
{
    for (auto* observer : mObservers)
    {
        delete observer;
    }
}

bool Observers::notifyAdd(CommandBuffer& commandBuffer, Entity entity, ColumnId column)
{
    bool triggered = false;

    auto range = mOnAdd.equal_range(column);
    for (auto it = range.first; it != range.second; ++it)
    {
        auto* observer = mObservers[it->second.inner];
        if (observer != nullptr)
        {
            observer->run({.cmdBuffer = commandBuffer, .observedEntity = entity});
            triggered = true;
        }
    }

    return triggered;
}

bool Observers::notifyRemove(CommandBuffer& commandBuffer, Entity entity, ColumnId column)
{
    bool triggered = false;

    auto range = mOnRemove.equal_range(column);
    for (auto it = range.first; it != range.second; ++it)
    {
        auto* observer = mObservers[it->second.inner];
        if (observer != nullptr)
        {
            observer->run({.cmdBuffer = commandBuffer, .observedEntity = entity});
            triggered = true;
        }
    }

    return triggered;
}

bool Observers::notifyDestroy(CommandBuffer& commandBuffer, Entity entity, ColumnId column)
{
    bool triggered = false;

    auto range = mOnDestroy.equal_range(column);
    for (auto it = range.first; it != range.second; ++it)
    {
        auto* observer = mObservers[it->second.inner];
        if (observer != nullptr)
        {
            observer->run({.cmdBuffer = commandBuffer, .observedEntity = entity});
            triggered = true;
        }
    }

    return triggered;
}

bool Observers::notifyRelate(CommandBuffer& commandBuffer, Entity entity, ColumnId column)
{
    bool triggered = false;

    auto range = mOnRelate.equal_range(column);
    for (auto it = range.first; it != range.second; ++it)
    {
        auto* observer = mObservers[it->second.inner];
        if (observer != nullptr)
        {
            observer->run({.cmdBuffer = commandBuffer, .observedEntity = entity});
            triggered = true;
        }
    }

    return triggered;
}

bool Observers::notifyUnrelate(CommandBuffer& commandBuffer, Entity entity, ColumnId column)
{
    bool triggered = false;

    auto range = mOnUnrelate.equal_range(column);
    for (auto it = range.first; it != range.second; ++it)
    {
        auto* observer = mObservers[it->second.inner];
        if (observer != nullptr)
        {
            observer->run({.cmdBuffer = commandBuffer, .observedEntity = entity});
            triggered = true;
        }
    }

    return triggered;
}


ObserverId Observers::hookOnAdd(ColumnId column, System<void> observer)
{
    ObserverId id{.inner = mObservers.size()};
    mObservers.push_back(new System<void>(std::move(observer)));
    mOnAdd.emplace(column, id);
    return id;
}

ObserverId Observers::hookOnRemove(ColumnId column, System<void> observer)
{
    ObserverId id{.inner = mObservers.size()};
    mObservers.push_back(new System<void>(std::move(observer)));
    mOnRemove.emplace(column, id);
    return id;
}

ObserverId Observers::hookOnDestroy(ColumnId column, System<void> observer)
{
    ObserverId id{.inner = mObservers.size()};
    mObservers.push_back(new System<void>(std::move(observer)));
    mOnDestroy.emplace(column, id);
    return id;
}

ObserverId Observers::hookOnRelate(ColumnId column, System<void> observer)
{
    ObserverId id{.inner = mObservers.size()};
    mObservers.push_back(new System<void>(std::move(observer)));
    mOnRelate.emplace(column, id);
    return id;
}

ObserverId Observers::hookOnUnrelate(ColumnId column, System<void> observer)
{
    ObserverId id{.inner = mObservers.size()};
    mObservers.push_back(new System<void>(std::move(observer)));
    mOnUnrelate.emplace(column, id);
    return id;
}

void Observers::unhook(ObserverId id)
{
    CUBOS_ASSERT(mObservers[id.inner] != nullptr);
    delete mObservers[id.inner];
    mObservers[id.inner] = nullptr;
}
