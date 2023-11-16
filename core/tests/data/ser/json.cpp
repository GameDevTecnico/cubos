#include <doctest/doctest.h>
#include <glm/vec3.hpp>

#include <cubos/core/data/ser/debug.hpp>
#include <cubos/core/data/ser/json.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/constructible_utils.hpp>
#include <cubos/core/reflection/traits/fields.hpp>

using cubos::core::data::JSONSerializer;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::Type;

#define AUTO_TEST(type, value, expected)                                                                               \
    ser.write<type>(value);                                                                                            \
    json = ser.output();                                                                                               \
    CHECK((json.dump()) == (expected));

TEST_CASE("data::JSONSerializer")
{
    JSONSerializer ser{};
    auto json = ser.output();

    /// [Primitive types]
    AUTO_TEST(bool, true, "true");
    AUTO_TEST(char, 'a', "97");
    AUTO_TEST(int8_t, -128, "-128");
    AUTO_TEST(int16_t, -32768, "-32768");
    AUTO_TEST(int32_t, -2147483648, "-2147483648");
    AUTO_TEST(int64_t, -9223372036854775807, "-9223372036854775807");
    AUTO_TEST(uint8_t, 255, "255");
    AUTO_TEST(uint16_t, 65535, "65535");
    AUTO_TEST(uint32_t, 4294967295, "4294967295");
    AUTO_TEST(uint64_t, 18446744073709551615ULL, "18446744073709551615");
    AUTO_TEST(float, 1.5F, "1.5");
    AUTO_TEST(double, 1.00000000000005, "1.00000000000005");

    /// [String]
    AUTO_TEST(std::string, "Hello, world!", "\"Hello, world!\"");

    /// [UUID]
    AUTO_TEST(uuids::uuid, *uuids::uuid::from_string("f7063cd4-de44-47b5-b0a9-f0e7a558c9e5"),
              "\"f7063cd4-de44-47b5-b0a9-f0e7a558c9e5\"");

    /// [Map]
    std::map<std::string, bool> map{{"false", false}, {"true", true}};
    AUTO_TEST(decltype(map), map, "{\"false\":false,\"true\":true}");

    /// [Vector]
    std::vector<int> vector{1, 2, 3};
    AUTO_TEST(decltype(vector), vector, "[1,2,3]");

    /// [glm::vec3]
    glm::ivec3 vec3{1, 2, 3};
    AUTO_TEST(decltype(vec3), vec3, "{\"x\":1,\"y\":2,\"z\":3}");

    /// [Nested types]
    std::vector<std::vector<std::vector<glm::ivec3>>> vec;
    std::vector<glm::ivec3> inner1 = {{1, 2, 3}, {4, 5, 6}};
    std::vector<glm::ivec3> inner2 = {{7, 8, 9}, {10, 11, 12}};
    std::vector<std::vector<glm::ivec3>> outer1;
    outer1.push_back(inner1);
    std::vector<std::vector<glm::ivec3>> outer2;
    outer2.push_back(inner2);
    vec.push_back(outer1);
    vec.push_back(outer2);
    AUTO_TEST(decltype(vec), vec,
              "[[[{\"x\":1,\"y\":2,\"z\":3},{\"x\":4,\"y\":5,\"z\":6}]],"
              "[[{\"x\":7,\"y\":8,\"z\":9},{\"x\":10,\"y\":11,\"z\":12}]]]");

    /// [Structs]
    struct ExampleStruct
    {
        CUBOS_REFLECT
        {
            return Type::create("ExampleStruct")
                .with(FieldsTrait{}
                          .withField("a", &ExampleStruct::a)
                          .withField("b", &ExampleStruct::b)
                          .withField("c", &ExampleStruct::c));
        }
        int a;
        float b;
        std::string c;
    };
    ExampleStruct myStruct{1, 2.5F, "Hello, world!"};
    AUTO_TEST(decltype(myStruct), myStruct, "{\"a\":1,\"b\":2.5,\"c\":\"Hello, world!\"}");
}