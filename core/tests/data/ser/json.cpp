#include <doctest/doctest.h>
#include <glm/vec3.hpp>

#include <cubos/core/data/ser/json.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/constructible_utils.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/mask.hpp>
#include <cubos/core/reflection/traits/nullable.hpp>

using cubos::core::data::JSONSerializer;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::MaskTrait;
using cubos::core::reflection::NullableTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;

#define AUTO_TEST(type, value, expected)                                                                               \
    ser.write<type>(value);                                                                                            \
    json = ser.output();                                                                                               \
    CHECK((json.dump()) == (expected));

namespace
{
    struct ExampleStruct
    {
        CUBOS_REFLECT;

        int a;
        float b;
        std::string c;
    };

    enum class Color
    {
        Red,
        Green,
        Blue
    };

    struct Nullable
    {
        CUBOS_REFLECT;

        uint32_t value;
    };

    enum class Permissions
    {
        None = 0,
        Read = 1,
        Write = 2,
        Execute = 4
    };

    inline Permissions operator~(Permissions p)
    {
        return static_cast<Permissions>(~static_cast<int>(p));
    }

    inline Permissions operator|(Permissions a, Permissions b)
    {
        return static_cast<Permissions>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline Permissions operator&(Permissions a, Permissions b)
    {
        return static_cast<Permissions>(static_cast<int>(a) & static_cast<int>(b));
    }
} // namespace

CUBOS_REFLECT_IMPL(ExampleStruct)
{
    return Type::create("ExampleStruct")
        .with(FieldsTrait{}
                  .withField("a", &ExampleStruct::a)
                  .withField("b", &ExampleStruct::b)
                  .withField("c", &ExampleStruct::c));
}

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, Color);
CUBOS_REFLECT_EXTERNAL_IMPL(Color)
{
    return Type::create("Color").with(
        EnumTrait{}.withVariant<Color::Red>("Red").withVariant<Color::Green>("Green").withVariant<Color::Blue>("Blue"));
}

CUBOS_REFLECT_IMPL(Nullable)
{
    return Type::create("Nullable")
        .with(NullableTrait{
            [](const void* instance) -> bool { return static_cast<const Nullable*>(instance)->value == UINT32_MAX; },
            [](void* instance) { static_cast<Nullable*>(instance)->value = UINT32_MAX; }});
}

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, Permissions);
CUBOS_REFLECT_EXTERNAL_IMPL(Permissions)
{
    return Type::create("Permissions")
        .with(MaskTrait{}
                  .withBit<Permissions::Read>("Read")
                  .withBit<Permissions::Write>("Write")
                  .withBit<Permissions::Execute>("Execute"));
}

TEST_CASE("data::JSONSerializer")
{
    JSONSerializer ser{};
    auto json = ser.output();

    // Boolean type.
    AUTO_TEST(bool, true, "true");

    // Char type (distinct from `signed char` and `unsigned char`).
    AUTO_TEST(char, 'a', "97");

    // Signed integer types.
    AUTO_TEST(signed char, -128, "-128");
    AUTO_TEST(short, -32768, "-32768");
    AUTO_TEST(int, -2147483648, "-2147483648");
    AUTO_TEST(long, -2147483648L, "-2147483648");
    AUTO_TEST(long long, -9223372036854775807LL, "-9223372036854775807");

    // Unsigned integer types.
    AUTO_TEST(unsigned char, 255, "255");
    AUTO_TEST(unsigned short, 65535, "65535");
    AUTO_TEST(unsigned int, 4294967295, "4294967295");
    AUTO_TEST(unsigned long, 4294967295UL, "4294967295");
    AUTO_TEST(unsigned long long, 18446744073709551615ULL, "18446744073709551615");

    // Floating point types.
    AUTO_TEST(float, 1.5F, "1.5");
    AUTO_TEST(double, 1.00000000000005, "1.00000000000005");

    // String
    AUTO_TEST(std::string, "Hello, world!", "\"Hello, world!\"");

    // Enum.
    AUTO_TEST(Color, Color::Red, "\"Red\"");
    AUTO_TEST(Color, Color::Green, "\"Green\"");
    AUTO_TEST(Color, Color::Blue, "\"Blue\"");

    // Mask.
    AUTO_TEST(Permissions, Permissions::None, "[]");
    AUTO_TEST(Permissions, Permissions::Read, "[\"Read\"]");
    AUTO_TEST(Permissions, Permissions::Write, "[\"Write\"]");
    AUTO_TEST(Permissions, Permissions::Execute, "[\"Execute\"]");
    AUTO_TEST(Permissions, Permissions::Read | Permissions::Write, "[\"Read\",\"Write\"]");
    AUTO_TEST(Permissions, Permissions::Read | Permissions::Execute, "[\"Read\",\"Execute\"]");
    AUTO_TEST(Permissions, Permissions::Write | Permissions::Execute, "[\"Write\",\"Execute\"]");

    // UUID
    AUTO_TEST(uuids::uuid, *uuids::uuid::from_string("f7063cd4-de44-47b5-b0a9-f0e7a558c9e5"),
              "\"f7063cd4-de44-47b5-b0a9-f0e7a558c9e5\"");

    // Map
    std::map<std::string, bool> map1{{"false", false}, {"true", true}};
    AUTO_TEST(decltype(map1), map1, "{\"false\":false,\"true\":true}");
    std::map<int, bool> map2{{0, false}, {1, true}};
    AUTO_TEST(decltype(map2), map2, "{\"0\":false,\"1\":true}");

    // Vector
    std::vector<int> vector{1, 2, 3};
    AUTO_TEST(decltype(vector), vector, "[1,2,3]");

    // glm::vec3
    glm::ivec3 vec3{1, 2, 3};
    AUTO_TEST(decltype(vec3), vec3, "{\"x\":1,\"y\":2,\"z\":3}");

    // Nested data types
    std::vector<std::vector<std::vector<glm::ivec3>>> vec{{{{1, 2, 3}, {4, 5, 6}}}, {{{7, 8, 9}, {10, 11, 12}}}};
    AUTO_TEST(decltype(vec), vec,
              "[[[{\"x\":1,\"y\":2,\"z\":3},{\"x\":4,\"y\":5,\"z\":6}]],"
              "[[{\"x\":7,\"y\":8,\"z\":9},{\"x\":10,\"y\":11,\"z\":12}]]]");

    // Custom data types
    ExampleStruct myStruct{1, 2.5F, "Hello, world!"};
    AUTO_TEST(decltype(myStruct), myStruct, "{\"a\":1,\"b\":2.5,\"c\":\"Hello, world!\"}");

    // Null
    const auto& nullable = reflect<Nullable>();
    const auto& nullableTrait = nullable.get<NullableTrait>();
    Nullable null{144};
    nullableTrait.setToNull(&null);
    AUTO_TEST(Nullable, null, "null");
}
