#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/primitive.hpp>

#include "../utils.hpp"

using cubos::core::reflection::PrimitiveType;
using cubos::core::reflection::reflect;

template <typename T>
static void test(const char* name)
{
    testTypeGetters<T, PrimitiveType>(name, name);
    testTypeDefaultConstructor<T>();
    testTypeMoveConstructor<T>();
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
