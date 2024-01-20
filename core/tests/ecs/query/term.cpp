#include <doctest/doctest.h>

#include <cubos/core/ecs/query/term.hpp>

#include "../utils.hpp"

using cubos::core::reflection::reflect;

using namespace cubos::core::ecs;

TEST_CASE("ecs::QueryTerm")
{
    Types types{};
    types.addComponent(reflect<IntegerComponent>());
    types.addComponent(reflect<ParentComponent>());
    auto integerComponent = types.id(reflect<IntegerComponent>());
    auto parentComponent = types.id(reflect<ParentComponent>());

    SUBCASE("make*")
    {
        auto term = QueryTerm::makeEntity(1);
        CHECK(term.isEntity());
        CHECK(term.type == DataTypeId::Invalid);
        CHECK(term.entity.target == 1);

        term = QueryTerm::makeWithComponent(integerComponent, 1);
        CHECK(term.isComponent(types));
        CHECK(term.type == integerComponent);
        CHECK(term.component.target == 1);
        CHECK_FALSE(term.component.without);
        CHECK_FALSE(term.component.optional);

        term = QueryTerm::makeWithoutComponent(integerComponent, 1);
        CHECK(term.isComponent(types));
        CHECK(term.type == integerComponent);
        CHECK(term.component.target == 1);
        CHECK(term.component.without);
        CHECK_FALSE(term.component.optional);

        term = QueryTerm::makeOptComponent(integerComponent, 1);
        CHECK(term.isComponent(types));
        CHECK(term.type == integerComponent);
        CHECK(term.component.target == 1);
        CHECK_FALSE(term.component.without);
        CHECK(term.component.optional);
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
        // below.
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
}
