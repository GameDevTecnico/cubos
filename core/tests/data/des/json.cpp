#include <limits>

#include <doctest/doctest.h>
#include <glm/vec2.hpp>

#include <cubos/core/data/des/json.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/variant.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>

using cubos::core::data::JSONDeserializer;

template <typename T>
static void test(JSONDeserializer& des, T expected)
{
    T actual{};
    des.feed(expected);
    REQUIRE(des.read(actual));
    REQUIRE(actual == expected);
}

template <typename T>
static void testNumber(JSONDeserializer& des)
{
    test<T>(des, static_cast<T>(0));
    test<T>(des, std::numeric_limits<T>::min());
    test<T>(des, std::numeric_limits<T>::max());
    test<T>(des, std::numeric_limits<T>::lowest());
}

TEST_CASE("data::JSONDeserializer")
{
    cubos::core::disableLogging();
    JSONDeserializer des{};

    SUBCASE("invalid json is reported as such")
    {
        REQUIRE_FALSE(des.parse("invalid json"));
    }

    SUBCASE("valid json is parsed correctly")
    {
        REQUIRE(des.parse("{}"));
    }

    SUBCASE("booleans are deserialized correctly")
    {
        test<bool>(des, true);
        test<bool>(des, false);
    }

    SUBCASE("numbers are deserialized correctly")
    {
        testNumber<int8_t>(des);
        testNumber<int16_t>(des);
        testNumber<int32_t>(des);
        testNumber<int64_t>(des);
        testNumber<uint8_t>(des);
        testNumber<uint16_t>(des);
        testNumber<uint32_t>(des);
        testNumber<uint64_t>(des);
        testNumber<float>(des);
        testNumber<double>(des);
    }

    SUBCASE("strings are deserialized correctly")
    {
        test<std::string>(des, "");
        test<std::string>(des, "string with \"quotes\" and\nmultiple lines");
    }

    SUBCASE("objects are deserialized correctly")
    {
        glm::ivec2 actual = {1, 2};

        SUBCASE("with all fields")
        {
            des.feed(nlohmann::json::object({
                {"x", 3},
                {"y", 4},
            }));

            REQUIRE(des.read(actual));
            CHECK(actual.x == 3);
            CHECK(actual.y == 4);
        }

        SUBCASE("with allowed missing fields")
        {
            des.allowsMissingFields(true);
            des.feed(nlohmann::json::object({{"x", 3}}));

            REQUIRE(des.read(actual));
            CHECK(actual.x == 3);
            CHECK(actual.y == 2);
        }

        SUBCASE("with non-allowed missing fields")
        {
            des.feed(nlohmann::json::object({{"x", 3}}));
            REQUIRE_FALSE(des.read(actual));
        }

        SUBCASE("with extra fields")
        {
            des.feed(nlohmann::json::object({
                {"x", 3},
                {"y", 4},
                {"z", 5},
            }));

            REQUIRE_FALSE(des.read(actual));
        }

        SUBCASE("with invalid fields")
        {
            des.feed(nlohmann::json::object({
                {"x", 3},
                {"y", "4"},
            }));

            REQUIRE_FALSE(des.read(actual));
        }
    }

    SUBCASE("variants are deserialized correctly")
    {
        std::variant<int32_t, bool> actual;

        SUBCASE("first variant")
        {
            des.feed(nlohmann::json::object({
                {"type", "int32_t"},
                {"value", 1},
            }));

            REQUIRE(des.read(actual));
            REQUIRE(actual.index() == 0);
            CHECK(std::get<int32_t>(actual) == 1);
        }

        SUBCASE("second variant")
        {
            des.feed(nlohmann::json::object({
                {"type", "bool"},
                {"value", true},
            }));

            REQUIRE(des.read(actual));
            REQUIRE(actual.index() == 1);
            CHECK(std::get<bool>(actual) == true);
        }

        SUBCASE("unknown variant")
        {
            des.feed(nlohmann::json::object({
                {"type", "double"},
                {"value", 1.2},
            }));

            REQUIRE_FALSE(des.read(actual));
        }

        SUBCASE("not a JSON object")
        {
            des.feed(nlohmann::json::array({}));
            REQUIRE_FALSE(des.read(actual));
        }

        SUBCASE("missing type field")
        {
            des.feed(nlohmann::json::object({
                {"value", 3},
            }));

            REQUIRE_FALSE(des.read(actual));
        }

        SUBCASE("missing value field")
        {
            des.feed(nlohmann::json::object({
                {"type", "int"},
            }));

            REQUIRE_FALSE(des.read(actual));
        }

        SUBCASE("with an extra field")
        {
            des.feed(nlohmann::json::object({
                {"type", "bool"},
                {"value", true},
                {"trash", 1},
            }));

            REQUIRE_FALSE(des.read(actual));
        }

        SUBCASE("with an invalid type")
        {
            des.feed(nlohmann::json::object({
                {"type", 1},
                {"value", true},
            }));

            REQUIRE_FALSE(des.read(actual));
        }

        SUBCASE("with an invalid value")
        {
            des.feed(nlohmann::json::object({
                {"type", "bool"},
                {"value", 1},
            }));

            REQUIRE_FALSE(des.read(actual));
        }
    }

    SUBCASE("arrays are deserialized correctly")
    {
        std::vector<std::string> actual = {"foo"};

        SUBCASE("not an array")
        {
            des.feed(nlohmann::json::object({}));
            REQUIRE_FALSE(des.read(actual));
        }

        SUBCASE("with an element of the wrong type")
        {
            des.feed(nlohmann::json::array({"a", 1, "b"}));
            REQUIRE_FALSE(des.read(actual));
        }

        SUBCASE("empty array")
        {
            des.feed(nlohmann::json::array({}));
            REQUIRE(des.read(actual));
            CHECK(actual.empty());
        }

        SUBCASE("with two elements")
        {
            des.feed(nlohmann::json::array({"bar", "baz"}));
            REQUIRE(des.read(actual));
            REQUIRE(actual.size() == 2);
            CHECK(actual[0] == "bar");
            CHECK(actual[1] == "baz");
        }
    }

    SUBCASE("dictionaries are deserialized correctly")
    {
        SUBCASE("dictionary from std::string to int32_t")
        {
            std::unordered_map<std::string, int32_t> actual = {{"foo", 1}};

            SUBCASE("not a dictionary")
            {
                des.feed(nlohmann::json::array({}));
                REQUIRE_FALSE(des.read(actual));
            }

            SUBCASE("with an element of the wrong type")
            {
                des.feed(nlohmann::json::object({
                    {"foo", 1},
                    {"bar", "baz"},
                }));

                REQUIRE_FALSE(des.read(actual));
            }

            SUBCASE("empty dictionary")
            {
                des.feed(nlohmann::json::object({}));
                REQUIRE(des.read(actual));
                CHECK(actual.empty());
            }

            SUBCASE("with two elements")
            {
                des.feed(nlohmann::json::object({
                    {"bar", 2},
                    {"baz", 3},
                }));

                REQUIRE(des.read(actual));
                REQUIRE(actual.size() == 2);
                CHECK(actual["bar"] == 2);
                CHECK(actual["baz"] == 3);
            }
        }

        SUBCASE("dictionary from int32_t to std::string")
        {
            std::unordered_map<int32_t, std::string> actual = {{1, "foo"}};

            SUBCASE("with a key of the wrong type")
            {
                des.feed(nlohmann::json::object({
                    {"foo", "bar"},
                    {"baz", "qux"},
                }));

                REQUIRE_FALSE(des.read(actual));
            }

            SUBCASE("with two elements")
            {
                des.feed(nlohmann::json::object({
                    {"2", "bar"},
                    {"3", "baz"},
                }));

                REQUIRE(des.read(actual));
                REQUIRE(actual.size() == 2);
                CHECK(actual[2] == "bar");
                CHECK(actual[3] == "baz");
            }
        }
    }
}
