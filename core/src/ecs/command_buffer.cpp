#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/world.hpp>
#include <cubos/core/memory/any_value.hpp>

using cubos::core::ecs::CommandBuffer;
using cubos::core::ecs::Entity;

CommandBuffer::CommandBuffer(World& world)
    : mWorld(world)
{
    // Do nothing.
}

Entity CommandBuffer::create()
{
    std::lock_guard<std::mutex> lock(mMutex);

    auto entity = mWorld.reserve();
    mCommands.emplace_back([entity](World& world) { world.createAt(entity); });
    return entity;
}

void CommandBuffer::destroy(Entity entity)
{
    std::lock_guard<std::mutex> lock(mMutex);

    mCommands.emplace_back([entity](World& world) { world.destroy(entity); });
}

std::unordered_map<std::string, Entity> CommandBuffer::spawn(const Blueprint& blueprint, bool withName)
{
    std::unordered_map<std::string, Entity> nameToEntity{};

    blueprint.instantiate(
        [&](const std::string& name) -> Entity {
            auto entity = this->create();
            nameToEntity.emplace(name, entity);
            return entity;
        },
        [&](Entity entity, memory::AnyValue component) { this->add(entity, component.type(), component.get()); },
        [&](Entity fromEntity, Entity toEntity, memory::AnyValue relation) {
            this->relate(fromEntity, toEntity, relation.type(), relation.get());
        },
        withName);

    return nameToEntity;
}

void CommandBuffer::add(Entity entity, const reflection::Type& type, void* value)
{
    std::lock_guard<std::mutex> lock(mMutex);

    mCommands.emplace_back([entity, value = memory::AnyValue::moveConstruct(type, value)](World& world) mutable {
        world.components(entity).add(value.type(), value.get());
    });
}

void CommandBuffer::remove(Entity entity, const reflection::Type& type)
{
    std::lock_guard<std::mutex> lock(mMutex);

    mCommands.emplace_back([entity, &type](World& world) { world.components(entity).remove(type); });
}

void CommandBuffer::relate(Entity from, Entity to, const reflection::Type& type, void* value)
{
    std::lock_guard<std::mutex> lock(mMutex);

    mCommands.emplace_back([from, to, value = memory::AnyValue::moveConstruct(type, value)](World& world) mutable {
        world.relate(from, to, value.type(), value.get());
    });
}

void CommandBuffer::unrelate(Entity from, Entity to, const reflection::Type& type)
{
    std::lock_guard<std::mutex> lock(mMutex);

    mCommands.emplace_back([from, to, &type](World& world) { world.unrelate(from, to, type); });
}

void CommandBuffer::push(memory::Function<void(World&)> command)
{
    std::lock_guard<std::mutex> lock(mMutex);

    mCommands.emplace_back(memory::move(command));
}

void CommandBuffer::commit()
{
    std::lock_guard<std::mutex> lock(mMutex);

    for (auto& cmd : mCommands)
    {
        cmd(mWorld);
    }

    mCommands.clear();
}
