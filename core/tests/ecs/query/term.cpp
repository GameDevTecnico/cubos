#include <doctest/doctest.h>

#include <cubos/core/ecs/query/term.hpp>

#include "../utils.hpp"

using cubos::core::reflection::reflect;

using namespace cubos::core::ecs;

// NOLINTBEGIN(readability-function-size)
TEST_CASE("ecs::QueryTerm")
{
    Types types{};
    types.addComponent(reflect<IntegerComponent>());
    types.addComponent(reflect<ParentComponent>());
    types.addRelation(reflect<EmptyRelation>());
    auto integerComponent = types.id(reflect<IntegerComponent>());
    auto parentComponent = types.id(reflect<ParentComponent>());
    auto emptyRelation = types.id(reflect<EmptyRelation>());

    SUBCASE("make*")
    {
        auto term1 = QueryTerm::makeEntity(1);
        CHECK(term1.isEntity());
        CHECK(term1.type == DataTypeId::Invalid);
        CHECK(term1.entity.target == 1);
        CHECK(term1.compare(types, term1));

        auto term2 = QueryTerm::makeWithComponent(integerComponent, 1);
        CHECK(term2.isComponent(types));
        CHECK(term2.type == integerComponent);
        CHECK(term2.component.target == 1);
        CHECK_FALSE(term2.component.without);
        CHECK_FALSE(term2.component.optional);
        CHECK(term2.compare(types, term2));
        CHECK_FALSE(term2.compare(types, term1));

        auto term3 = QueryTerm::makeWithoutComponent(integerComponent, 1);
        CHECK(term3.isComponent(types));
        CHECK(term3.type == integerComponent);
        CHECK(term3.component.target == 1);
        CHECK(term3.component.without);
        CHECK_FALSE(term3.component.optional);
        CHECK(term3.compare(types, term3));
        CHECK_FALSE(term3.compare(types, term2));
        CHECK_FALSE(term3.compare(types, term1));

        auto term4 = QueryTerm::makeOptComponent(integerComponent, 1);
        CHECK(term4.isComponent(types));
        CHECK(term4.type == integerComponent);
        CHECK(term4.component.target == 1);
        CHECK_FALSE(term4.component.without);
        CHECK(term4.component.optional);
        CHECK(term4.compare(types, term4));
        CHECK_FALSE(term4.compare(types, term3));
        CHECK_FALSE(term4.compare(types, term2));
        CHECK_FALSE(term4.compare(types, term1));

        auto term5 = QueryTerm::makeRelation(emptyRelation, 1, 2);
        CHECK(term5.isRelation(types));
        CHECK(term5.type == emptyRelation);
        CHECK(term5.relation.fromTarget == 1);
        CHECK(term5.relation.toTarget == 2);
        CHECK(term5.compare(types, term5));
        CHECK_FALSE(term5.compare(types, term4));
        CHECK_FALSE(term5.compare(types, term3));
        CHECK_FALSE(term5.compare(types, term2));
        CHECK_FALSE(term5.compare(types, term1));
    }

    SUBCASE("empty resolve")
    {
        std::vector<QueryTerm> otherTerms = {};
        auto result = QueryTerm::resolve(types, {}, otherTerms);
        CHECK(result.empty());
        CHECK(otherTerms.empty());
    }

    SUBCASE("resolve without other terms")
    {
        // Equivalent to having Query<>, with the manual terms in the vector below.
        std::vector<QueryTerm> otherTerms = {};
        auto result = QueryTerm::resolve(types,
                                         {
                                             QueryTerm::makeEntity(-1),
                                             QueryTerm::makeOptComponent(integerComponent, -1),
                                             QueryTerm::makeEntity(1),
                                             QueryTerm::makeWithComponent(integerComponent, -1),
                                             QueryTerm::makeWithoutComponent(parentComponent, -1),
                                         },
                                         otherTerms);
        REQUIRE(result.size() == 5);

        REQUIRE(result[0].isEntity());
        REQUIRE(result[0].entity.target == 0);

        REQUIRE(result[1].isComponent(types));
        REQUIRE(result[1].component.target == 0);
        REQUIRE_FALSE(result[1].component.without);
        REQUIRE(result[1].component.optional);

        REQUIRE(result[2].isEntity());
        REQUIRE(result[2].entity.target == 1);

        REQUIRE(result[3].isComponent(types));
        REQUIRE(result[3].component.target == 1);
        REQUIRE_FALSE(result[3].component.without);
        REQUIRE_FALSE(result[3].component.optional);

        REQUIRE(result[4].isComponent(types));
        REQUIRE(result[4].component.target == 1);
        REQUIRE(result[4].component.without);
        REQUIRE_FALSE(result[4].component.optional);

        CHECK(otherTerms.empty());
    }

    SUBCASE("resolve without base terms")
    {
        // Equivalent to having Query<Opt<IntegerComponent&>, Entity, ParentComponent&>, without any manual terms.
        std::vector<QueryTerm> otherTerms = {
            QueryTerm::makeOptComponent(integerComponent, -1),
            QueryTerm::makeEntity(-1),
            QueryTerm::makeWithComponent(parentComponent, -1),
        };
        auto result = QueryTerm::resolve(types, {}, otherTerms);
        REQUIRE(result.size() == 3);

        REQUIRE(result[0].isComponent(types));
        REQUIRE(result[0].component.target == 0);
        REQUIRE_FALSE(result[0].component.without);
        REQUIRE(result[0].component.optional);

        REQUIRE(result[1].isEntity());
        REQUIRE(result[1].entity.target == 0);

        REQUIRE(result[2].isComponent(types));
        REQUIRE(result[2].component.target == 0);
        REQUIRE_FALSE(result[2].component.without);
        REQUIRE_FALSE(result[2].component.optional);

        REQUIRE(otherTerms.size() == 3);
        REQUIRE(otherTerms[0].component.target == 0);
        REQUIRE(otherTerms[1].entity.target == 0);
        REQUIRE(otherTerms[2].component.target == 0);
    }

    SUBCASE("resolve with both types of terms")
    {
        // Equivalent to having Query<Opt<IntegerComponent&>, Entity, IntegerComponent&, Entity>, with the manual terms
        // Integer(0), Integer(1)
        std::vector<QueryTerm> otherTerms = {
            QueryTerm::makeOptComponent(integerComponent, -1),
            QueryTerm::makeEntity(-1),
            QueryTerm::makeWithComponent(integerComponent, -1),
            QueryTerm::makeEntity(-1),
        };
        auto result = QueryTerm::resolve(types,
                                         {
                                             QueryTerm::makeWithComponent(integerComponent, 0),
                                             QueryTerm::makeWithComponent(integerComponent, 1),
                                         },
                                         otherTerms);
        REQUIRE(result.size() == 4);

        REQUIRE(result[0].isComponent(types));
        REQUIRE(result[0].component.target == 0);
        REQUIRE_FALSE(result[0].component.without);
        REQUIRE(result[0].component.optional);

        REQUIRE(result[1].isEntity());
        REQUIRE(result[1].entity.target == 0);

        REQUIRE(result[2].isComponent(types));
        REQUIRE(result[2].component.target == 1);
        REQUIRE_FALSE(result[2].component.without);
        REQUIRE_FALSE(result[2].component.optional);

        REQUIRE(result[3].isEntity());
        REQUIRE(result[3].entity.target == 1);

        REQUIRE(otherTerms.size() == 4);
        REQUIRE(otherTerms[0].component.target == 0);
        REQUIRE(otherTerms[1].entity.target == 0);
        REQUIRE(otherTerms[2].component.target == 1);
        REQUIRE(otherTerms[3].entity.target == 1);
    }

    SUBCASE("resolve with relations but without base terms")
    {
        // Equivalent to having Query<Entity, EmptyRelation&, Entity, EmptyRelation& Entity> with no manual terms.
        std::vector<QueryTerm> otherTerms = {
            QueryTerm::makeEntity(-1), QueryTerm::makeRelation(emptyRelation, -1, -1),
            QueryTerm::makeEntity(-1), QueryTerm::makeRelation(emptyRelation, -1, -1),
            QueryTerm::makeEntity(-1),
        };
        auto result = QueryTerm::resolve(types, {}, otherTerms);

        REQUIRE(result.size() == 5);

        REQUIRE(result[0].isEntity());
        REQUIRE(result[0].entity.target == 0);

        REQUIRE(result[1].isRelation(types));
        REQUIRE(result[1].relation.fromTarget == 0);
        REQUIRE(result[1].relation.toTarget == 1);

        REQUIRE(result[2].isEntity());
        REQUIRE(result[2].entity.target == 1);

        REQUIRE(result[3].isRelation(types));
        REQUIRE(result[3].relation.fromTarget == 1);
        REQUIRE(result[3].relation.toTarget == 2);

        REQUIRE(result[4].isEntity());
        REQUIRE(result[4].entity.target == 2);

        REQUIRE(otherTerms.size() == 5);
        REQUIRE(otherTerms[0].entity.target == 0);
        REQUIRE(otherTerms[1].relation.fromTarget == 0);
        REQUIRE(otherTerms[1].relation.toTarget == 1);
        REQUIRE(otherTerms[2].entity.target == 1);
        REQUIRE(otherTerms[3].relation.fromTarget == 1);
        REQUIRE(otherTerms[3].relation.toTarget == 2);
        REQUIRE(otherTerms[4].entity.target == 2);
    }

    SUBCASE("resolve with relation but with base terms")
    {
        // Equivalent to having Query<Entity, Entity> with the manual terms Entity(0), EmptyRelation(0, 1), Entity(1).
        std::vector<QueryTerm> otherTerms = {
            QueryTerm::makeEntity(-1),
            QueryTerm::makeEntity(-1),
        };
        auto result = QueryTerm::resolve(types,
                                         {
                                             QueryTerm::makeEntity(0),
                                             QueryTerm::makeRelation(emptyRelation, 0, 1),
                                             QueryTerm::makeEntity(1),
                                         },
                                         otherTerms);

        REQUIRE(result.size() == 3);

        REQUIRE(result[0].isEntity());
        REQUIRE(result[0].entity.target == 0);

        REQUIRE(result[1].isRelation(types));
        REQUIRE(result[1].relation.fromTarget == 0);
        REQUIRE(result[1].relation.toTarget == 1);

        REQUIRE(result[2].isEntity());
        REQUIRE(result[2].entity.target == 1);

        REQUIRE(otherTerms.size() == 2);
        REQUIRE(otherTerms[0].entity.target == 0);
        REQUIRE(otherTerms[1].entity.target == 1);
    }
}
// NOLINTEND(readability-function-size)
