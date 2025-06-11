#include <string>

#include <doctest/doctest.h>

#include <cubos/core/reflection/external/const.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::reflection::ConstantTrait;
using cubos::core::reflection::reflect;

template <typename T>
static void test(const char* name)
{
    CHECK(reflect<const T>().name() == name);
    CHECK(reflect<const T>().template has<ConstantTrait>());
    CHECK(reflect<const T>().template get<ConstantTrait>().type().template is<T>());
}

TEST_CASE("reflection::reflect<const T>()")
{
    test<int>("const int");
    test<float>("const float");
    test<std::string>("const std::string");
}
