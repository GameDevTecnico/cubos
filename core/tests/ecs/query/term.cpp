#include <doctest/doctest.h>

#include <cubos/core/ecs/query/term.hpp>

#include "../utils.hpp"

using cubos::core::reflection::reflect;

using namespace cubos::core::ecs;

TEST_CASE("ecs::QueryTerm")
{
    Types types{};
    types.addComponent(reflect<IntegerComponent>());

    auto term = QueryTerm::makeEntity(1);
    CHECK(term.isEntity());
    CHECK(term.type == DataTypeId::Invalid);
    CHECK(term.entity.target == 1);

    term = QueryTerm::makeWithComponent(types.id(reflect<IntegerComponent>()), 1);
    CHECK(term.isComponent(types));
    CHECK(term.type == types.id(reflect<IntegerComponent>()));
    CHECK(term.component.target == 1);
    CHECK_FALSE(term.component.without);
    CHECK_FALSE(term.component.optional);

    term = QueryTerm::makeWithoutComponent(types.id(reflect<IntegerComponent>()), 1);
    CHECK(term.isComponent(types));
    CHECK(term.type == types.id(reflect<IntegerComponent>()));
    CHECK(term.component.target == 1);
    CHECK(term.component.without);
    CHECK_FALSE(term.component.optional);

    term = QueryTerm::makeOptComponent(types.id(reflect<IntegerComponent>()), 1);
    CHECK(term.isComponent(types));
    CHECK(term.type == types.id(reflect<IntegerComponent>()));
    CHECK(term.component.target == 1);
    CHECK_FALSE(term.component.without);
    CHECK(term.component.optional);
}
