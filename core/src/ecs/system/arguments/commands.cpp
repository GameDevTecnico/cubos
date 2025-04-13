#include <cubos/core/ecs/blueprint.hpp>
#include <cubos/core/ecs/command_buffer.hpp>
#include <cubos/core/ecs/name.hpp>
#include <cubos/core/ecs/system/arguments/commands.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;

Commands::Commands(CommandBuffer& buffer)
    : mBuffer{buffer}
{
}

void Commands::insertResource(memory::AnyValue value)
{
    mBuffer.insertResource(memory::move(value));
}

void Commands::eraseResource(const reflection::Type& type)
{
    mBuffer.eraseResource(type);
}

Commands::EntityBuilder Commands::create()
{
    auto entity = mBuffer.create();
    return {mBuffer, entity};
}

void Commands::destroy(Entity entity)
{
    mBuffer.destroy(entity);
}

Commands::BlueprintBuilder Commands::spawn(const Blueprint& blueprint)
{
    auto nameToEntity = mBuffer.spawn(blueprint);
    return {mBuffer, std::move(nameToEntity)};
}

Commands& Commands::add(Entity entity, const reflection::Type& type, void* value)
{
    mBuffer.add(entity, type, value);
    return *this;
}

Commands& Commands::remove(Entity entity, const reflection::Type& type)
{
    mBuffer.remove(entity, type);
    return *this;
}

Commands& Commands::relate(Entity from, Entity to, const reflection::Type& type, void* value)
{
    mBuffer.relate(from, to, type, value);
    return *this;
}

Commands& Commands::unrelate(Entity from, Entity to, const reflection::Type& type)
{
    mBuffer.unrelate(from, to, type);
    return *this;
}

Commands::EntityBuilder::EntityBuilder(CommandBuffer& buffer, Entity entity)
    : mBuffer(buffer)
    , mEntity(entity)
{
    // Do nothing.
}

Entity Commands::EntityBuilder::entity() const
{
    return mEntity;
}

Commands::EntityBuilder& Commands::EntityBuilder::named(std::string name)
{
    return this->add(Name{std::move(name)});
}

Commands::EntityBuilder& Commands::EntityBuilder::add(const reflection::Type& type, void* value)
{
    mBuffer.add(mEntity, type, value);
    return *this;
}

Commands::EntityBuilder& Commands::EntityBuilder::relatedFrom(Entity entity, const reflection::Type& type, void* value)
{
    mBuffer.relate(entity, mEntity, type, value);
    return *this;
}

Commands::EntityBuilder& Commands::EntityBuilder::relatedTo(Entity entity, const reflection::Type& type, void* value)
{
    mBuffer.relate(mEntity, entity, type, value);
    return *this;
}
Commands::BlueprintBuilder::BlueprintBuilder(CommandBuffer& buffer,
                                             std::unordered_map<std::string, Entity> nameToEntity)
    : mBuffer{buffer}
    , mNameToEntity{std::move(nameToEntity)}
{
}

Entity Commands::BlueprintBuilder::entity(const std::string& name) const
{
    CUBOS_ASSERT(mNameToEntity.contains(name), "No such entity {} in blueprint", name);
    return mNameToEntity.at(name);
}

Entity Commands::BlueprintBuilder::entity() const
{
    CUBOS_ASSERT(mNameToEntity.contains(""), "No root entity in blueprint");
    return mNameToEntity.at("");
}

Commands::BlueprintBuilder& Commands::BlueprintBuilder::add(const std::string& name, const reflection::Type& type,
                                                            void* value)
{
    mBuffer.add(this->entity(name), type, value);
    return *this;
}

Commands::BlueprintBuilder& Commands::BlueprintBuilder::add(const reflection::Type& type, void* value)
{
    mBuffer.add(this->entity(), type, value);
    return *this;
}

Commands::BlueprintBuilder& Commands::BlueprintBuilder::named(const std::string& name)
{
    for (auto& [entityName, entity] : mNameToEntity)
    {
        this->add(entityName, Name{name + entityName});
    }
    return *this;
}
