#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include "../utils.hpp"

using cubos::core::reflection::ArrayType;
using cubos::core::reflection::reflect;

template <typename T>
static void testStaticInfo(const char* name)
{
    testTypeGetters<std::vector<T>, ArrayType>(name, "std::vector");
    testTypeDefaultConstructor<std::vector<T>>();

    // Check if it holds the correct element type.
    auto& ta = reflect<std::vector<T>>().template asKind<ArrayType>();
    REQUIRE(ta.elementType().template is<T>());
}

TEST_CASE("reflection::reflect<std::vector>()")
{
    SUBCASE("static information is correct")
    {
        testStaticInfo<int32_t>("std::vector<int32_t>");
        testStaticInfo<std::vector<float>>("std::vector<std::vector<float>>");
    }

    SUBCASE("inspect and modify a vector")
    {
        std::vector<int32_t> vec = {1, 3};
        auto& t = reflect<decltype(vec)>().asKind<ArrayType>();

        // Check if the vector contains the correct elements, and if they can be modified.
        REQUIRE(t.length(&vec) == 2);
        CHECK(t.element<int32_t>(&vec, 0) == 1);
        CHECK(t.element<int32_t>(&vec, 1) == 3);
        t.element<int32_t>(&vec, 0) = 2;
        CHECK(t.element<int32_t>(&vec, 0) == 2);
        CHECK(t.element<int32_t>(&vec, 1) == 3);

        // Add one element to the vector.
        t.resize(&vec, 3);
        CHECK(t.length(&vec) == 3);
        CHECK(t.element<int32_t>(&vec, 2) == int32_t{});

        // Resize the vector to a single element.
        t.resize(&vec, 1);
        CHECK(t.length(&vec) == 1);
        CHECK(t.element<int32_t>(&vec, 0) == 2);

        // Clear the vector.
        t.resize(&vec, 0);
        CHECK(t.length(&vec) == 0);
    }
}
