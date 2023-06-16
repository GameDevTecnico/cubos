#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/variant.hpp>

#include "../utils.hpp"

using cubos::core::reflection::reflect;
using cubos::core::reflection::VariantType;

template <typename... TArgs>
static void testStaticInfo(const char* name)
{
    testTypeGetters<std::variant<TArgs...>, VariantType>(name, "std::variant");
    testTypeDefaultConstructor<std::variant<TArgs...>>();

    // Check if it holds the correct number of variants.
    auto& tv = reflect<std::variant<TArgs...>>().template asKind<VariantType>();
    CHECK(tv.variants().size() == sizeof...(TArgs));
}

TEST_CASE("reflection::reflect<std::variant>()")
{
    SUBCASE("static information is correct")
    {
        testStaticInfo<int32_t>("std::variant<int32_t>");
        testStaticInfo<bool, char, double>("std::variant<bool, char, double>");
        testStaticInfo<bool, std::variant<char>>("std::variant<bool, std::variant<char>>");
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
