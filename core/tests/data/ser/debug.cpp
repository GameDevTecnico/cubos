#include <doctest/doctest.h>
#include <glm/vec3.hpp>

#include <cubos/core/data/ser/debug.hpp>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include "../utils.hpp"

using cubos::core::data::DebugSerializer;
using cubos::core::data::Serializer;
using cubos::core::memory::BufferStream;
using cubos::core::memory::SeekOrigin;
using cubos::core::reflection::Type;

namespace
{
    struct Empty
    {
        CUBOS_REFLECT;
    };
} // namespace

CUBOS_REFLECT_IMPL(Empty)
{
    return Type::create("Empty");
}

#define AUTO_TEST(type, value, expected, success)                                                                      \
    stream.seek(0, SeekOrigin::Begin);                                                                                 \
    CHECK(ser.write<type>(value) == (success));                                                                        \
    stream.put('\0');                                                                                                  \
    stream.seek(0, SeekOrigin::Begin);                                                                                 \
    CHECK(dump(stream) == (expected));

#define AUTO_TEST_SPLIT(type, value, expectedUgly, expectedPretty, success)                                            \
    if (pretty)                                                                                                        \
    {                                                                                                                  \
        AUTO_TEST(type, value, expectedPretty, success);                                                               \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        AUTO_TEST(type, value, expectedUgly, success);                                                                 \
    }

TEST_CASE("data::DebugSerializer")
{
    BufferStream stream{};
    DebugSerializer ser{stream};

    bool pretty = false;
    PARAMETRIZE_TRUE_OR_FALSE("pretty", pretty);
    ser.pretty(pretty);

    AUTO_TEST(bool, false, "false", true);
    AUTO_TEST(bool, true, "true", true);
    AUTO_TEST(char, 'a', "'a'", true);
    AUTO_TEST(int8_t, -120, "-120", true);
    AUTO_TEST(int16_t, 30000, "30000", true);
    AUTO_TEST(int32_t, -2000000000, "-2000000000", true);
    AUTO_TEST(int64_t, 9000000000000000000, "9000000000000000000", true);
    AUTO_TEST(uint8_t, 120, "120", true);
    AUTO_TEST(uint16_t, 60000, "60000", true);
    AUTO_TEST(uint32_t, 4000000000, "4000000000", true);
    AUTO_TEST(uint64_t, 18000000000000000000ULL, "18000000000000000000", true);
    AUTO_TEST(float, 1.5F, "1.5000", true);
    AUTO_TEST(double, 1.F, "1.0000", true);
    AUTO_TEST(std::string, "Hello, world!", "\"Hello, world!\"", true);
    AUTO_TEST(uuids::uuid, *uuids::uuid::from_string("f7063cd4-de44-47b5-b0a9-f0e7a558c9e5"),
              "\"f7063cd4-de44-47b5-b0a9-f0e7a558c9e5\"", true);

    std::map<std::string, bool> map{{"false", false}, {"true", true}};
    AUTO_TEST_SPLIT(decltype(map), map, "{\"false\": false, \"true\": true}",
                    "{\n  \"false\": false,\n  \"true\": true\n}", true);

    std::vector<int> vector{1, 2, 3};
    AUTO_TEST_SPLIT(decltype(vector), vector, "[1, 2, 3]", "[\n  1,\n  2,\n  3\n]", true);

    glm::tvec3<int> vec3{1, 2, 3};
    AUTO_TEST_SPLIT(decltype(vec3), vec3, "(x: 1, y: 2, z: 3)", "(\n  x: 1,\n  y: 2,\n  z: 3\n)", true);

    ser.hook<std::string>([](auto) { return false; });
    ser.hook<bool>([](auto) { return false; });
    ser.hook<int>([](auto) { return false; });
    AUTO_TEST_SPLIT(decltype(map), map, "{?: ?, ?: ?}", "{\n  ?: ?,\n  ?: ?\n}", false);
    AUTO_TEST_SPLIT(decltype(vector), vector, "[?, ?, ?]", "[\n  ?,\n  ?,\n  ?\n]", false);
    AUTO_TEST_SPLIT(decltype(vec3), vec3, "(x: ?, y: ?, z: ?)", "(\n  x: ?,\n  y: ?,\n  z: ?\n)", false);

    AUTO_TEST_SPLIT(Empty, Empty{}, "?", "?", false);
}
