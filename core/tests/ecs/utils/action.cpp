#include "action.hpp"
#include <utility>

#include <doctest/doctest.h>

#include <cubos/core/ecs/query/filter.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/fields.hpp>

using cubos::core::ecs::QueryFilter;
using cubos::core::ecs::QueryTerm;
using cubos::core::ecs::SymmetricTrait;
using cubos::core::ecs::TreeTrait;

using namespace test::ecs;

static std::string constructor(Entity entity)
{
    return "{" + std::to_string(entity.index) + ", " + std::to_string(entity.generation) + "}";
}

static std::string constructor(const Type& type)
{
    return "world.types().type(world.types().id(\"" + type.name() + "\"))";
}

static std::string constructor(const std::unordered_set<const Type*>& types)
{
    std::string result = "{";
    for (const auto* type : types)
    {
        result += "&" + ::constructor(*type) + ", ";
    }
    result += "}";
    return result;
}

static std::size_t randomIndex(std::size_t size)
{
    if (size == 0)
    {
        return 0;
    }

    return static_cast<std::size_t>(rand()) % size;
}

Action* CreateAction::random(ExpectedWorld& /*expected*/)
{
    return new CreateAction();
}

std::string CreateAction::constructor() const
{
    return "CreateAction{}";
}

void CreateAction::test(World& world, ExpectedWorld& expected)
{
    auto entity = world.create();
    REQUIRE(world.isAlive(entity));

    // The identifier should be unique.
    REQUIRE_FALSE(expected.aliveEntities.contains(entity));
    REQUIRE_FALSE(expected.deadEntities.contains(entity));

    expected.aliveEntities[entity] = {};
    expected.testEntity(world, entity);
}

DestroyAction::DestroyAction(Entity entity)
    : mEntity(entity)
{
}

Action* DestroyAction::random(ExpectedWorld& expected)
{
    if (expected.aliveEntities.empty())
    {
        return nullptr;
    }

    auto it = expected.aliveEntities.begin();
    std::advance(it, randomIndex(expected.aliveEntities.size()));
    return new DestroyAction(it->first);
}

std::string DestroyAction::constructor() const
{
    return "DestroyAction{" + ::constructor(mEntity) + "}";
}

void DestroyAction::test(World& world, ExpectedWorld& expected)
{
    world.destroy(mEntity);

    // We also need to remove the relations which this entity is a part of.
    for (auto& [type, incoming] : expected.aliveEntities[mEntity].incoming)
    {
        for (auto& [from, value] : incoming)
        {
            expected.aliveEntities[from].outgoing[type].erase(mEntity);
        }
    }

    for (auto& [type, outgoing] : expected.aliveEntities[mEntity].outgoing)
    {
        for (auto& [to, value] : outgoing)
        {
            expected.aliveEntities[to].incoming[type].erase(mEntity);
        }
    }

    expected.aliveEntities.erase(mEntity);
    expected.deadEntities.insert(mEntity);
    expected.testEntity(world, mEntity);
}

AddAction::AddAction(Entity entity, const Type& type, int value)
    : mEntity(entity)
    , mType(type)
    , mValue(value)
{
}

Action* AddAction::random(ExpectedWorld& expected)
{
    if (expected.aliveEntities.empty())
    {
        return nullptr;
    }

    auto it = expected.aliveEntities.begin();
    std::advance(it, randomIndex(expected.aliveEntities.size()));
    const auto& entity = it->first;

    if (expected.componentTypes.empty())
    {
        return nullptr;
    }

    auto typeIt = expected.componentTypes.begin();
    std::advance(typeIt, randomIndex(expected.componentTypes.size()));
    auto* type = *typeIt;

    return new AddAction(entity, *type, rand());
}

std::string AddAction::constructor() const
{
    return "AddAction{" + ::constructor(mEntity) + ", " + ::constructor(mType) + ", " + std::to_string(mValue) + "}";
}

void AddAction::test(World& world, ExpectedWorld& expected)
{
    ExpectedInteger component{mValue};
    world.components(mEntity).add(mType, &component);
    expected.aliveEntities[mEntity].components[&mType] = mValue;
    expected.testEntity(world, mEntity);
}

RemoveAction::RemoveAction(Entity entity, const Type& type)
    : mEntity(entity)
    , mType(type)
{
}

Action* RemoveAction::random(ExpectedWorld& expected)
{
    if (expected.aliveEntities.empty())
    {
        return nullptr;
    }

    auto it = expected.aliveEntities.begin();
    std::advance(it, randomIndex(expected.aliveEntities.size()));
    const auto& entity = it->first;

    if (it->second.components.empty())
    {
        return nullptr;
    }

    auto typeIt = it->second.components.begin();
    std::advance(typeIt, randomIndex(it->second.components.size()));
    const auto* type = typeIt->first;

    return new RemoveAction(entity, *type);
}

std::string RemoveAction::constructor() const
{
    return "RemoveAction{" + ::constructor(mEntity) + ", " + ::constructor(mType) + "}";
}

void RemoveAction::test(World& world, ExpectedWorld& expected)
{
    world.components(mEntity).remove(mType);
    expected.aliveEntities[mEntity].components.erase(&mType);
    expected.testEntity(world, mEntity);
}

RelateAction::RelateAction(Entity from, Entity to, const Type& type, int value)
    : mFrom(from)
    , mTo(to)
    , mType(type)
    , mValue(value)
{
}

Action* RelateAction::random(ExpectedWorld& expected)
{
    if (expected.relationTypes.empty())
    {
        return nullptr;
    }

    auto typeIt = expected.relationTypes.begin();
    std::advance(typeIt, randomIndex(expected.relationTypes.size()));
    const auto* type = *typeIt;

    if (expected.aliveEntities.size() < 2)
    {
        return nullptr;
    }

    Entity from;
    Entity to;

    do
    {
        auto it = expected.aliveEntities.begin();
        std::advance(it, randomIndex(expected.aliveEntities.size()));
        from = it->first;

        if (expected.aliveEntities.empty())
        {
            return nullptr;
        }

        auto toIt = expected.aliveEntities.begin();
        std::advance(toIt, randomIndex(expected.aliveEntities.size()));
        to = toIt->first;

        // We want to avoid creating cycles in tree relations, so we only accept relations where the 'from' entity has a
        // lower index than the 'to' entity.
    } while (from.index >= to.index && type->has<TreeTrait>());

    return new RelateAction(from, to, *type, rand());
}

std::string RelateAction::constructor() const
{
    return "RelateAction{" + ::constructor(mFrom) + ", " + ::constructor(mTo) + ", " + ::constructor(mType) + ", " +
           std::to_string(mValue) + "}";
}

void RelateAction::test(World& world, ExpectedWorld& expected)
{
    ExpectedInteger relation{mValue};
    world.relate(mFrom, mTo, mType, &relation);

    if (mType.has<TreeTrait>())
    {
        // Then we overwrite any previous outgoing relations.
        for (auto [to, value] : expected.aliveEntities[mFrom].outgoing[&mType])
        {
            expected.aliveEntities[to].incoming[&mType].erase(mFrom);
        }

        expected.aliveEntities[mFrom].outgoing[&mType].clear();
    }

    if (mType.has<SymmetricTrait>())
    {
        expected.aliveEntities[mFrom].incoming[&mType][mTo] = mValue;
        expected.aliveEntities[mTo].outgoing[&mType][mFrom] = mValue;
    }

    expected.aliveEntities[mFrom].outgoing[&mType][mTo] = mValue;
    expected.aliveEntities[mTo].incoming[&mType][mFrom] = mValue;

    expected.testEntity(world, mFrom);
    expected.testEntity(world, mTo);
}

UnrelateAction::UnrelateAction(Entity from, Entity to, const Type& type)
    : mFrom(from)
    , mTo(to)
    , mType(type)
{
}

Action* UnrelateAction::random(ExpectedWorld& expected)
{
    if (expected.aliveEntities.empty())
    {
        return nullptr;
    }

    auto it = expected.aliveEntities.begin();
    std::advance(it, randomIndex(expected.aliveEntities.size()));
    const auto& from = it->first;

    if (expected.aliveEntities.empty())
    {
        return nullptr;
    }

    auto toIt = expected.aliveEntities.begin();
    std::advance(toIt, randomIndex(expected.aliveEntities.size()));
    const auto& to = toIt->first;

    if (expected.relationTypes.empty())
    {
        return nullptr;
    }

    auto typeIt = expected.relationTypes.begin();
    std::advance(typeIt, randomIndex(expected.relationTypes.size()));
    const auto* type = *typeIt;

    return new UnrelateAction(from, to, *type);
}

std::string UnrelateAction::constructor() const
{
    return "UnrelateAction{" + ::constructor(mFrom) + ", " + ::constructor(mTo) + ", " + ::constructor(mType) + "}";
}

void UnrelateAction::test(World& world, ExpectedWorld& expected)
{
    world.unrelate(mFrom, mTo, mType);

    if (mType.has<SymmetricTrait>())
    {
        expected.aliveEntities[mFrom].incoming[&mType].erase(mTo);
        expected.aliveEntities[mTo].outgoing[&mType].erase(mFrom);
    }

    expected.aliveEntities[mFrom].outgoing[&mType].erase(mTo);
    expected.aliveEntities[mTo].incoming[&mType].erase(mFrom);
    expected.testEntity(world, mFrom);
    expected.testEntity(world, mTo);
}

SingleTargetQueryAction::SingleTargetQueryAction(std::unordered_set<const Type*> with,
                                                 std::unordered_set<const Type*> without,
                                                 std::unordered_set<const Type*> optional)
    : mWith(std::move(with))
    , mWithout(std::move(without))
    , mOptional(std::move(optional))
{
}

Action* SingleTargetQueryAction::random(ExpectedWorld& expected)
{
    std::unordered_set<const Type*> candidates;
    std::unordered_set<const Type*> with;
    std::unordered_set<const Type*> without;
    std::unordered_set<const Type*> optional;

    for (const auto* type : expected.componentTypes)
    {
        candidates.insert(type);
    }

    auto termCount = randomIndex(candidates.size()) + 1;

    // Randomly select which terms are 'with', 'without' and 'optional'.
    auto withCount = randomIndex(termCount - 1) + 1;
    while (with.size() < withCount)
    {
        auto it = candidates.begin();
        std::advance(it, randomIndex(candidates.size()));
        with.insert(*it);
        candidates.erase(it);
    }

    auto optionalCount = randomIndex(termCount - withCount);
    auto withoutCount = termCount - withCount - optionalCount;
    while (without.size() < withoutCount)
    {
        auto it = candidates.begin();
        std::advance(it, randomIndex(candidates.size()));
        without.insert(*it);
        candidates.erase(it);
    }

    while (optional.size() < optionalCount)
    {
        auto it = candidates.begin();
        std::advance(it, randomIndex(candidates.size()));
        optional.insert(*it);
        candidates.erase(it);
    }

    return new SingleTargetQueryAction(with, without, optional);
}

std::string SingleTargetQueryAction::constructor() const
{
    return "SingleTargetQueryAction{" + ::constructor(mWith) + ", " + ::constructor(mWithout) + ", " +
           ::constructor(mOptional) + "}";
}

void SingleTargetQueryAction::test(World& world, ExpectedWorld& expected)
{
    std::vector<QueryTerm> terms;
    for (const auto* type : mWith)
    {
        auto dataTypeId = world.types().id(*type);
        terms.emplace_back(QueryTerm::makeWithComponent(dataTypeId, 0));
    }
    for (const auto* type : mWithout)
    {
        auto dataTypeId = world.types().id(*type);
        terms.emplace_back(QueryTerm::makeWithoutComponent(dataTypeId, 0));
    }
    for (const auto* type : mOptional)
    {
        auto dataTypeId = world.types().id(*type);
        terms.emplace_back(QueryTerm::makeOptComponent(dataTypeId, 0));
    }
    QueryFilter filter{world, terms};

    // Get all entities matched by the query.
    std::unordered_set<Entity, EntityHash> entities;
    for (auto match : filter.view())
    {
        REQUIRE_FALSE(entities.contains(match.entities[0]));
        entities.insert(match.entities[0]);
    }

    // Check if all entities which should be matched are matched.
    for (const auto& [entity, expectedEntity] : expected.aliveEntities)
    {
        bool shouldMatch = true;
        for (const auto* type : mWith)
        {
            if (!expectedEntity.components.contains(type))
            {
                shouldMatch = false;
                break;
            }
        }
        for (const auto* type : mWithout)
        {
            if (expectedEntity.components.contains(type))
            {
                shouldMatch = false;
                break;
            }
        }

        if (shouldMatch)
        {
            REQUIRE(entities.contains(entity));
            expected.testEntity(world, entity);
            entities.erase(entity);
        }
        else
        {
            REQUIRE_FALSE(entities.contains(entity));
        }
    }

    // Make sure that no entities remain.
    REQUIRE(entities.empty());
}

SingleRelationQueryAction::SingleRelationQueryAction(const Type& relation, std::unordered_set<const Type*> withA,
                                                     std::unordered_set<const Type*> withoutA,
                                                     std::unordered_set<const Type*> optionalA,
                                                     std::unordered_set<const Type*> withB,
                                                     std::unordered_set<const Type*> withoutB,
                                                     std::unordered_set<const Type*> optionalB)
    : mRelation{relation}
    , mWithA(std::move(withA))
    , mWithoutA(std::move(withoutA))
    , mOptionalA(std::move(optionalA))
    , mWithB(std::move(withB))
    , mWithoutB(std::move(withoutB))
    , mOptionalB(std::move(optionalB))
{
}

Action* SingleRelationQueryAction::random(ExpectedWorld& expected)
{
    if (expected.relationTypes.empty())
    {
        return nullptr;
    }

    auto typeIt = expected.relationTypes.begin();
    std::advance(typeIt, randomIndex(expected.relationTypes.size()));
    const auto* type = *typeIt;

    std::unordered_set<const Type*> withA;
    std::unordered_set<const Type*> withoutA;
    std::unordered_set<const Type*> optionalA;
    std::unordered_set<const Type*> withB;
    std::unordered_set<const Type*> withoutB;
    std::unordered_set<const Type*> optionalB;

    {
        std::unordered_set<const Type*> candidates;

        for (const auto* type : expected.componentTypes)
        {
            candidates.insert(type);
        }

        auto termCount = randomIndex(candidates.size()) + 1;

        // Randomly select which terms are 'with', 'without' and 'optional'.
        auto withCount = randomIndex(termCount - 1) + 1;
        while (withA.size() < withCount)
        {
            auto it = candidates.begin();
            std::advance(it, randomIndex(candidates.size()));
            withA.insert(*it);
            candidates.erase(it);
        }

        auto optionalCount = randomIndex(termCount - withCount);
        auto withoutCount = termCount - withCount - optionalCount;
        while (withoutA.size() < withoutCount)
        {
            auto it = candidates.begin();
            std::advance(it, randomIndex(candidates.size()));
            withoutA.insert(*it);
            candidates.erase(it);
        }

        while (optionalA.size() < optionalCount)
        {
            auto it = candidates.begin();
            std::advance(it, randomIndex(candidates.size()));
            optionalA.insert(*it);
            candidates.erase(it);
        }
    }

    {
        std::unordered_set<const Type*> candidates;

        for (const auto* type : expected.componentTypes)
        {
            candidates.insert(type);
        }

        auto termCount = randomIndex(candidates.size()) + 1;

        // Randomly select which terms are 'with', 'without' and 'optional'.
        auto withCount = randomIndex(termCount - 1) + 1;
        while (withB.size() < withCount)
        {
            auto it = candidates.begin();
            std::advance(it, randomIndex(candidates.size()));
            withB.insert(*it);
            candidates.erase(it);
        }

        auto optionalCount = randomIndex(termCount - withCount);
        auto withoutCount = termCount - withCount - optionalCount;
        while (withoutB.size() < withoutCount)
        {
            auto it = candidates.begin();
            std::advance(it, randomIndex(candidates.size()));
            withoutB.insert(*it);
            candidates.erase(it);
        }

        while (optionalB.size() < optionalCount)
        {
            auto it = candidates.begin();
            std::advance(it, randomIndex(candidates.size()));
            optionalB.insert(*it);
            candidates.erase(it);
        }
    }

    return new SingleRelationQueryAction(*type, withA, withoutA, optionalA, withB, withoutB, optionalB);
}

std::string SingleRelationQueryAction::constructor() const
{
    return "SingleRelationQueryAction{" + ::constructor(mRelation) + ", " + ::constructor(mWithA) + ", " +
           ::constructor(mWithoutA) + ", " + ::constructor(mOptionalA) + ", " + ::constructor(mWithB) + ", " +
           ::constructor(mWithoutB) + ", " + ::constructor(mOptionalB) + "}";
}

void SingleRelationQueryAction::test(World& world, ExpectedWorld& expected)
{
    std::vector<QueryTerm> terms;
    for (const auto* type : mWithA)
    {
        auto dataTypeId = world.types().id(*type);
        terms.emplace_back(QueryTerm::makeWithComponent(dataTypeId, 0));
    }
    for (const auto* type : mWithoutA)
    {
        auto dataTypeId = world.types().id(*type);
        terms.emplace_back(QueryTerm::makeWithoutComponent(dataTypeId, 0));
    }
    for (const auto* type : mOptionalA)
    {
        auto dataTypeId = world.types().id(*type);
        terms.emplace_back(QueryTerm::makeOptComponent(dataTypeId, 0));
    }

    terms.emplace_back(QueryTerm::makeRelation(world.types().id(mRelation), 0, 1));

    for (const auto* type : mWithB)
    {
        auto dataTypeId = world.types().id(*type);
        terms.emplace_back(QueryTerm::makeWithComponent(dataTypeId, 1));
    }
    for (const auto* type : mWithoutB)
    {
        auto dataTypeId = world.types().id(*type);
        terms.emplace_back(QueryTerm::makeWithoutComponent(dataTypeId, 1));
    }
    for (const auto* type : mOptionalB)
    {
        auto dataTypeId = world.types().id(*type);
        terms.emplace_back(QueryTerm::makeOptComponent(dataTypeId, 1));
    }

    bool excludeDuplicates = mWithA.size() == mWithB.size() && mWithoutA.size() == mWithoutB.size() &&
                             mOptionalA.size() == mOptionalB.size();
    for (const auto* type : mWithA)
    {
        if (mWithB.contains(type))
        {
            excludeDuplicates = false;
            break;
        }
    }
    for (const auto* type : mWithoutA)
    {
        if (mWithoutB.contains(type))
        {
            excludeDuplicates = false;
            break;
        }
    }
    for (const auto* type : mOptionalA)
    {
        if (mOptionalB.contains(type))
        {
            excludeDuplicates = false;
            break;
        }
    }

    QueryFilter filter{world, terms};

    // Get all entities matched by the query.
    std::unordered_multimap<Entity, Entity, EntityHash> entities;
    for (auto match : filter.view())
    {
        auto range = entities.equal_range(match.entities[0]);

        if (excludeDuplicates)
        {
            for (auto it = range.first; it != range.second; ++it)
            {
                // Multiple matches for the same pair of entities should not exist.
                REQUIRE(it->second != match.entities[1]);
            }
        }

        entities.insert({match.entities[0], match.entities[1]});
    }

    std::unordered_multimap<Entity, Entity, EntityHash> alreadyMatched{};

    // Check if all entities which should be matched are matched.
    for (auto& [entity, expectedEntity] : expected.aliveEntities)
    {
        bool shouldMatch = true;
        for (const auto* type : mWithA)
        {
            if (!expectedEntity.components.contains(type))
            {
                shouldMatch = false;
                break;
            }
        }
        for (const auto* type : mWithoutA)
        {
            if (expectedEntity.components.contains(type))
            {
                shouldMatch = false;
                break;
            }
        }

        if (!shouldMatch)
        {
            REQUIRE_FALSE(entities.contains(entity));
            continue;
        }

        for (const auto& [toEntity, value] : expectedEntity.outgoing[&mRelation])
        {
            bool matches = true;
            const auto& toExpectedEntity = expected.aliveEntities[toEntity];
            for (const auto* type : mWithB)
            {
                if (!toExpectedEntity.components.contains(type))
                {
                    matches = false;
                    break;
                }
            }

            for (const auto* type : mWithoutB)
            {
                if (toExpectedEntity.components.contains(type))
                {
                    matches = false;
                    break;
                }
            }

            // If the relation is symmetric, we must make sure that we didn't already match the relation in the
            // opposite direction.
            if (mRelation.has<SymmetricTrait>())
            {
                auto range = alreadyMatched.equal_range(toEntity);
                for (auto it = range.first; it != range.second; ++it)
                {
                    if (it->second == entity)
                    {
                        matches = false;
                        break;
                    }
                }

                if (matches)
                {
                    alreadyMatched.insert({entity, toEntity});
                }
            }

            if (matches)
            {
                auto range = entities.equal_range(entity);
                auto it = range.first;
                for (; it != range.second; ++it)
                {
                    if (it->second == toEntity)
                    {
                        break;
                    }
                }

                if (it == range.second && mRelation.has<SymmetricTrait>())
                {
                    range = entities.equal_range(toEntity);
                    it = range.first;
                    for (; it != range.second; ++it)
                    {
                        if (it->second == entity)
                        {
                            break;
                        }
                    }
                }

                // Make sure that the matched relation actually exists.
                REQUIRE(it != range.second);
                entities.erase(it);

                if (entity == toEntity && !excludeDuplicates && mRelation.has<SymmetricTrait>())
                {
                    // Then it should have been matched twice.
                    range = entities.equal_range(entity);
                    it = range.first;
                    for (; it != range.second; ++it)
                    {
                        if (it->second == toEntity)
                        {
                            break;
                        }
                    }

                    // Make sure that the matched relation actually exists.
                    REQUIRE(it != range.second);
                    entities.erase(it);
                }
            }
        }
    }

    // Make sure that no entities remain.
    REQUIRE(entities.empty());
}
