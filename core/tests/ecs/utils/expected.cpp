#include "expected.hpp"
#include <utility>

#include <doctest/doctest.h>

#include <cubos/core/ecs/query/filter.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::ecs::QueryFilter;
using cubos::core::ecs::QueryTerm;
using cubos::core::ecs::SymmetricTrait;
using cubos::core::ecs::TreeTrait;

using namespace test::ecs;

Type& ExpectedInteger::createType(std::string name)
{
    using namespace cubos::core::reflection;
    return Type::create(std::move(name))
        .with(FieldsTrait{}.withField("value", &ExpectedInteger::value))
        .with(ConstructibleTrait::Builder<ExpectedInteger>{}.withBasicConstructors().build());
}

ExpectedWorld::~ExpectedWorld()
{
    for (auto* type : componentTypes)
    {
        Type::destroy(*type);
    }

    for (auto* type : relationTypes)
    {
        Type::destroy(*type);
    }
}

const Type& ExpectedWorld::registerComponent(std::string name)
{
    auto& type = ExpectedInteger::createType(std::move(name));
    componentTypes.insert(&type);
    return type;
}

const Type& ExpectedWorld::registerRelation(std::string name)
{
    auto& type = ExpectedInteger::createType(std::move(name));
    relationTypes.insert(&type);
    return type;
}

const Type& ExpectedWorld::registerSymmetricRelation(std::string name)
{
    auto& type = ExpectedInteger::createType(std::move(name)).with(SymmetricTrait{});
    relationTypes.insert(&type);
    return type;
}

const Type& ExpectedWorld::registerTreeRelation(std::string name)
{
    auto& type = ExpectedInteger::createType(std::move(name)).with(TreeTrait{});
    relationTypes.insert(&type);
    return type;
}

void ExpectedWorld::testTypes(World& world)
{
    // Check if the world has the expected component types.
    for (auto* type : componentTypes)
    {
        REQUIRE(world.types().contains(type->name()));
        auto dataTypeId = world.types().id(*type);
        REQUIRE(world.types().id(type->name()) == dataTypeId);
        REQUIRE(world.types().isComponent(dataTypeId));
        REQUIRE_FALSE(world.types().isRelation(dataTypeId));
    }

    // Check if the world has the expected relation types.
    for (auto* type : relationTypes)
    {
        REQUIRE(world.types().contains(type->name()));
        auto dataTypeId = world.types().id(*type);
        REQUIRE(world.types().id(type->name()) == dataTypeId);
        REQUIRE(world.types().isRelation(dataTypeId));
        REQUIRE_FALSE(world.types().isComponent(dataTypeId));
    }
}

void ExpectedWorld::testEntity(World& world, Entity entity)
{
    if (aliveEntities.contains(entity))
    {
        const auto& expectedEntity = aliveEntities.at(entity);
        REQUIRE(world.isAlive(entity));
        REQUIRE(world.generation(entity.index) == entity.generation);

        // Check if all components are correct.
        for (const auto& [componentType, expectedValue] : expectedEntity.components)
        {
            REQUIRE(world.components(entity).has(*componentType));
            const auto* data = world.components(entity).get(*componentType);
            REQUIRE(static_cast<const ExpectedInteger*>(data)->value == expectedValue);
        }

        // Make sure that there are no more components than expected.
        for (const auto& component : world.components(entity))
        {
            REQUIRE(expectedEntity.components.find(component.type) != expectedEntity.components.end());
        }

        // Make sure that all expected relations are present.
        for (const auto& [relationType, expectedRelations] : expectedEntity.incoming)
        {
            for (const auto& [fromEntity, expectedValue] : expectedRelations)
            {
                REQUIRE(world.relationsTo(entity).has(*relationType, fromEntity));
                const auto* data = world.relationsTo(entity).get(*relationType, fromEntity);
                REQUIRE(static_cast<const ExpectedInteger*>(data)->value == expectedValue);
            }
        }

        for (const auto& [relationType, expectedRelations] : expectedEntity.outgoing)
        {
            for (const auto& [toEntity, expectedValue] : expectedRelations)
            {
                REQUIRE(world.relationsFrom(entity).has(*relationType, toEntity));
                const auto* data = world.relationsFrom(entity).get(*relationType, toEntity);
                REQUIRE(static_cast<const ExpectedInteger*>(data)->value == expectedValue);
            }
        }

        // Make sure that there are no more relations than expected.
        for (const auto& [type, data, fromEntity] : world.relationsTo(entity))
        {
            REQUIRE(expectedEntity.incoming.find(type) != expectedEntity.incoming.end());
            REQUIRE(expectedEntity.incoming.at(type).find(fromEntity) != expectedEntity.incoming.at(type).end());
        }

        for (const auto& [type, data, toEntity] : world.relationsFrom(entity))
        {
            REQUIRE(expectedEntity.outgoing.find(type) != expectedEntity.outgoing.end());
            REQUIRE(expectedEntity.outgoing.at(type).find(toEntity) != expectedEntity.outgoing.at(type).end());
        }
    }
    else
    {
        REQUIRE_FALSE(world.isAlive(entity));
    }
}

void ExpectedWorld::testEntities(World& world)
{
    for (const auto& entity : aliveEntities)
    {
        this->testEntity(world, entity.first);
    }

    for (const auto& entity : deadEntities)
    {
        this->testEntity(world, entity);
    }
}
