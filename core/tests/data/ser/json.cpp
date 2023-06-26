#include <doctest/doctest.h>
#include <glm/vec2.hpp>

#include <cubos/core/data/ser/json.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/external/variant.hpp>
#include <cubos/core/reflection/external/vector.hpp>

using cubos::core::data::JSONSerializer;

template <typename K, typename V>
using Map = std::unordered_map<K, V>;

template <typename T>
static std::string toJSON(const T& value, int indent = -1)
{
    JSONSerializer ser{};
    ser.write(value);
    return ser.string(indent);
}

TEST_CASE("data::JSONSerializer")
{
    SUBCASE("booleans are serialized correctly")
    {
        CHECK(toJSON(false) == "false");
        CHECK(toJSON(true) == "true");
    }

    SUBCASE("numbers are serialized correctly")
    {
        CHECK(toJSON<int8_t>(1) == "1");
        CHECK(toJSON<int16_t>(-320) == "-320");
        CHECK(toJSON<int32_t>(293842) == "293842");
        CHECK(toJSON<int64_t>(-19438189481) == "-19438189481");
        CHECK(toJSON<uint8_t>(0) == "0");
        CHECK(toJSON<uint16_t>(42328) == "42328");
        CHECK(toJSON<uint32_t>(2432824243) == "2432824243");
        CHECK(toJSON<uint64_t>(18446744073709551615) == "18446744073709551615");
        CHECK(toJSON<float>(1.0F) == "1.0");
        CHECK(toJSON<double>(2.0) == "2.0");
    }

    SUBCASE("strings are serialized correctly")
    {
        CHECK(toJSON<std::string>("") == "\"\"");
        CHECK(toJSON<std::string>("hello\nworld") == "\"hello\\nworld\"");
    }

    SUBCASE("objects are serialized correctly")
    {
        CHECK(toJSON<glm::ivec2>({1, 2}) == "{\"x\":1,\"y\":2}");
        CHECK(toJSON<glm::ivec2>({1, 2}, 2) == "{\n  \"x\": 1,\n  \"y\": 2\n}");
    }

    SUBCASE("variants are serialized correctly")
    {
        CHECK(toJSON<std::variant<int32_t, std::string>>(1) == "{\"type\":\"int32_t\",\"value\":1}");
        CHECK(toJSON<std::variant<int32_t, std::string>>("hello") == "{\"type\":\"std::string\",\"value\":\"hello\"}");
    }

    SUBCASE("arrays are serialized correctly")
    {
        CHECK(toJSON<std::vector<int>>({1, 2, 3}) == "[1,2,3]");
        CHECK(toJSON<std::vector<int>>({1, 2, 3}, 2) == "[\n  1,\n  2,\n  3\n]");
    }

    SUBCASE("dictionaries are serialized correctly")
    {
        CHECK(toJSON<Map<std::string, int>>({{"a", 1}, {"b", 2}}) == "{\"a\":1,\"b\":2}");
        CHECK(toJSON<Map<std::string, int>>({{"a", 1}, {"b", 2}}, 2) == "{\n  \"a\": 1,\n  \"b\": 2\n}");
        CHECK(toJSON<Map<int, std::string>>({{1, "a"}, {2, "b"}}) == "{\"1\":\"a\",\"2\":\"b\"}");
        CHECK(toJSON<Map<bool, std::string>>({{true, "a"}, {false, "b"}}) == "{\"false\":\"b\",\"true\":\"a\"}");
    }
}
