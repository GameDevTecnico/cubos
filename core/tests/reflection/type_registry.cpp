#include <doctest/doctest.h>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/core/reflection/type_registry.hpp>

using cubos::core::reflection::TypeRegistry;

TEST_CASE("reflection::Type")
{
    TypeRegistry registry{};

    CHECK_FALSE(registry.contains("int"));
    CHECK_FALSE(registry.contains<int>());
    CHECK(registry.size() == 0);
    CHECK(registry.begin() == registry.end());

    SUBCASE("single insert")
    {
        registry.insert<int>();
        CHECK(registry.size() == 1);
    }

    SUBCASE("single insert other registry")
    {
        TypeRegistry other{};
        other.insert<int>();
        registry.insert(other);
        CHECK(registry.size() == 1);
    }

    SUBCASE("double insert")
    {
        registry.insert<int>();
        registry.insert<int>();
        CHECK(registry.size() == 1);
    }

    SUBCASE("double insert other registry")
    {
        TypeRegistry other{};
        other.insert<int>();
        registry.insert(other);
        registry.insert(other);
        CHECK(registry.size() == 1);
    }

    SUBCASE("with other types")
    {
        registry.insert<short>();
        registry.insert<int>();
        registry.insert<long>();
        registry.insert<float>();
        registry.insert<double>();
        CHECK(registry.size() == 5);
    }

    if (registry.size() == 1)
    {
        CHECK(registry.begin() != registry.end());
        CHECK(++registry.begin() == registry.end());
        CHECK(registry.begin()->first->is<int>());
    }

    REQUIRE(registry.contains("int"));
    CHECK(registry.at("int").is<int>());
}
