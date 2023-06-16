#include <doctest/doctest.h>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/primitive.hpp>

using cubos::core::reflection::PrimitiveType;
using cubos::core::reflection::reflect;

template <typename T>
static void test(const char* name)
{
    auto& t = reflect<T>();
    CHECK(t.template isKind<PrimitiveType>());
    CHECK(t.template is<T>());
    CHECK(t.name() == name);
    CHECK(t.shortName() == name);

    // Check if the default constructor really creates a default instance of the type.
    auto* instance = t.defaultConstruct();
    CHECK(*static_cast<T*>(instance) == T{});
    t.destroy(instance);
}

TEST_CASE("reflection::reflect<(external/primitives)>()")
{
    test<bool>("bool");
    test<char>("char");
    test<int8_t>("int8_t");
    test<int16_t>("int16_t");
    test<int32_t>("int32_t");
    test<int64_t>("int64_t");
    test<uint8_t>("uint8_t");
    test<uint16_t>("uint16_t");
    test<uint32_t>("uint32_t");
    test<uint64_t>("uint64_t");
    test<float>("float");
    test<double>("double");
}

TEST_CASE("reflection::reflect<std::string>()")
{
    test<std::string>("std::string");
}
