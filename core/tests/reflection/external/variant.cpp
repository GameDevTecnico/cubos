#include <doctest/doctest.h>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/variant.hpp>

using cubos::core::reflection::reflect;
using cubos::core::reflection::VariantType;

template <typename... TArgs>
static void getters(const char* name)
{
    auto& t = reflect<std::variant<TArgs...>>();
    CHECK(t.template isKind<VariantType>());
    CHECK(t.template is<std::variant<TArgs...>>());
    CHECK(t.name() == name);
    CHECK(t.shortName() == "std::variant");

    // Check if it holds the correct number of variants.
    auto& tv = t.template asKind<VariantType>();
    CHECK(tv.variants().size() == sizeof...(TArgs));

    // Check if the default constructor really creates a default instance of the type.
    auto* instance = t.defaultConstruct();
    CHECK(*static_cast<std::variant<TArgs...>*>(instance) == std::variant<TArgs...>{});
    t.destroy(instance);
}

TEST_CASE("reflection::reflect<std::variant>()")
{
    SUBCASE("type getters")
    {
        getters<int32_t>("std::variant<int32_t>");
        getters<bool, char, double>("std::variant<bool, char, double>");
        getters<bool, std::variant<char>>("std::variant<bool, std::variant<char>>");
    }

    SUBCASE("inspect and modify a variant")
    {
        std::variant<bool, char, int32_t> var = 'a';
        auto& t = reflect<decltype(var)>().asKind<VariantType>();

        // Check which variant the instance holds.
        REQUIRE(t.index(&var) == 1);
        auto& vChar = t.variant(&var);
        REQUIRE(&vChar == &t.variant<char>());
        REQUIRE(&vChar.type() == &reflect<char>());

        // Check if its value is correct and if it can be modified.
        CHECK(vChar.get<char>(&var) == 'a');
        vChar.get<char>(&var) = 'b';
        CHECK(std::get<char>(var) == 'b');

        // Change to another variant.
        auto& vBool = t.variant<bool>();
        REQUIRE(&vBool.type() == &reflect<bool>());
        vBool.emplaceDefault(&var);
        REQUIRE(&t.variant(&var) == &vBool);
        REQUIRE(vBool.get<bool>(&var) == bool{});
    }
}
