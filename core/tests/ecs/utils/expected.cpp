#include "expected.hpp"
#include <utility>

#include <doctest/doctest.h>

#include <cubos/core/ecs/query/filter.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::ecs::QueryFilter;
using cubos::core::ecs::QueryTerm;
using cubos::core::ecs::SymmetricTrait;

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

        // Check if all relations are correct.
        for (const auto& [relationType, expectedRelations] : expectedEntity.incoming)
        {
            auto dataTypeId = world.types().id(*relationType);
            QueryFilter filter{world, {QueryTerm::makeRelation(dataTypeId, 0, 1)}};
            for (auto match : filter.view().pin(1, entity))
            {
                REQUIRE(match.entities[1] == entity);
                REQUIRE(expectedRelations.contains(match.entities[0]));
                const auto* data = world.relation(match.entities[0], match.entities[1], *relationType);
                REQUIRE(static_cast<const ExpectedInteger*>(data)->value == expectedRelations.at(match.entities[0]));
            }

            // Make sure that all expected relations are present.
            for (const auto& [fromEntity, expectedValue] : expectedRelations)
            {
                REQUIRE(world.related(fromEntity, entity, *relationType));
            }
        }

        for (const auto& [relationType, expectedRelations] : expectedEntity.outgoing)
        {
            auto dataTypeId = world.types().id(*relationType);
            QueryFilter filter{world, {QueryTerm::makeRelation(dataTypeId, 0, 1)}};
            for (auto match : filter.view().pin(0, entity))
            {
                REQUIRE(match.entities[0] == entity);
                REQUIRE(expectedRelations.contains(match.entities[1]));
                const auto* data = world.relation(match.entities[0], match.entities[1], *relationType);
                REQUIRE(static_cast<const ExpectedInteger*>(data)->value == expectedRelations.at(match.entities[1]));
            }

            // Make sure that all expected relations are present.
            for (const auto& [toEntity, expectedValue] : expectedRelations)
            {
                REQUIRE(world.related(entity, toEntity, *relationType));
            }
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
