#include <doctest/doctest.h>
#include <glm/vec3.hpp>

#include <cubos/core/data/des/json.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/constructible_utils.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/nullable.hpp>
#include <cubos/core/tel/logging.hpp>

#include "../utils.hpp"

using cubos::core::data::JSONDeserializer;
using cubos::core::reflection::autoConstructibleTrait;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::NullableTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;
using cubos::core::tel::Level;
using cubos::core::tel::Logger;

namespace
{
    struct NonConstructible
    {
        CUBOS_REFLECT;

        int operator<=>(const NonConstructible& /*unused*/) const
        {
            return 0;
        }
    };

    struct Empty
    {
        CUBOS_REFLECT;

        int operator<=>(const Empty& /*unused*/) const
        {
            return 0;
        }
    };

    struct Wrapper
    {
        CUBOS_REFLECT;

        bool operator==(const Wrapper& /*unused*/) const = default;

        bool inner;
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

        bool operator==(const Nullable& /*unused*/) const = default;

        uint32_t value;
    };
} // namespace

CUBOS_REFLECT_IMPL(NonConstructible)
{
    return Type::create("NonConstructible");
}

CUBOS_REFLECT_IMPL(Empty)
{
    return Type::create("Empty").with(autoConstructibleTrait<Empty>());
}

CUBOS_REFLECT_IMPL(Wrapper)
{
    return Type::create("Wrapper").with(FieldsTrait{}.withField("inner", &Wrapper::inner));
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
        .with(FieldsTrait{}.withField("value", &Nullable::value))
        .with(NullableTrait{
            [](const void* instance) -> bool { return static_cast<const Nullable*>(instance)->value == UINT32_MAX; },
            [](void* instance) { static_cast<Nullable*>(instance)->value = UINT32_MAX; }});
}

#define AUTO_EXISTING(json, initial, expected)                                                                         \
    {                                                                                                                  \
        des.feed(json);                                                                                                \
        decltype(expected) value = (initial);                                                                          \
        CHECK(des.read(value));                                                                                        \
        CHECK(value == (expected));                                                                                    \
    }

#define AUTO_SUCCESS(json, expected)                                                                                   \
    {                                                                                                                  \
        des.feed(json);                                                                                                \
        decltype(expected) value{};                                                                                    \
        CHECK(des.read(value));                                                                                        \
        CHECK(value == (expected));                                                                                    \
    }

#define AUTO_FAILURE(json, ...)                                                                                        \
    {                                                                                                                  \
        des.feed(json);                                                                                                \
        __VA_ARGS__ value{};                                                                                           \
        CHECK_FALSE(des.read(value));                                                                                  \
    }

// NOLINTBEGIN(readability-function-size)
TEST_CASE("data::JSONDeserializer")
{
    using Json = nlohmann::json;

    cubos::core::tel::level(Level::Critical);

    JSONDeserializer des{};

    AUTO_SUCCESS(true, true);
    AUTO_SUCCESS(false, false);
    AUTO_SUCCESS(-120, static_cast<int8_t>(-120));
    AUTO_SUCCESS(30000, static_cast<int16_t>(30000));
    AUTO_SUCCESS(-2000000000, static_cast<int32_t>(-2000000000));
    AUTO_SUCCESS(9000000000000000000, static_cast<int64_t>(9000000000000000000));
    AUTO_SUCCESS(240, static_cast<uint8_t>(240));
    AUTO_SUCCESS(60000, static_cast<uint16_t>(60000));
    AUTO_SUCCESS(4000000000, static_cast<uint32_t>(4000000000));
    AUTO_SUCCESS(18000000000000000000ULL, static_cast<uint64_t>(18000000000000000000ULL));
    AUTO_SUCCESS(1.5F, 1.5F);
    AUTO_SUCCESS(1.5, 1.5);
    AUTO_SUCCESS("Hello, world!", std::string{"Hello, world!"});
    AUTO_SUCCESS("f7063cd4-de44-47b5-b0a9-f0e7a558c9e5",
                 *uuids::uuid::from_string("f7063cd4-de44-47b5-b0a9-f0e7a558c9e5"));
    AUTO_SUCCESS("Red", Color::Red);
    AUTO_SUCCESS("Green", Color::Green);
    AUTO_SUCCESS("Blue", Color::Blue);

    AUTO_FAILURE("true", bool);
    AUTO_FAILURE("-120", int8_t);
    AUTO_FAILURE(1.5F, std::string);
    AUTO_FAILURE("?", uuids::uuid);
    AUTO_FAILURE("?", Empty);
    AUTO_FAILURE("White", Color);

    AUTO_EXISTING((Json::object({})), (std::map<std::string, bool>{{"true", true}}), (std::map<std::string, bool>{}));
    AUTO_SUCCESS((Json::object({})), (std::map<std::string, bool>{}));
    AUTO_SUCCESS((Json::object({{"false", false}, {"true", true}})),
                 (std::map<std::string, bool>{{"false", false}, {"true", true}}));
    AUTO_SUCCESS((Json::object({{"false", true}, {"true", false}})),
                 (std::map<bool, bool>{{false, true}, {true, false}}));
    AUTO_SUCCESS(
        (Json::object({{"f7063cd4-de44-47b5-b0a9-f0e7a558c9e5", true}})),
        (std::map<uuids::uuid, bool>{{*uuids::uuid::from_string("f7063cd4-de44-47b5-b0a9-f0e7a558c9e5"), true}}));
    AUTO_FAILURE((Json::object({{"yes", true}})), std::map<bool, bool>);
    AUTO_FAILURE((Json::object({{"true", "yes"}})), std::map<bool, bool>);
    AUTO_FAILURE((Json::object({})), std::map<NonConstructible, bool>);
    AUTO_FAILURE((Json::object({{"?", true}})), std::map<Empty, bool>);
    AUTO_FAILURE((Json::object({{"?", true}})), std::map<uuids::uuid, bool>);

    AUTO_EXISTING(true, Wrapper{false}, Wrapper{true});
    AUTO_SUCCESS(false, Wrapper{false});
    AUTO_FAILURE((Json::object({{"inner", "true"}})), Wrapper);

    AUTO_EXISTING((Json::object({{"x", 4}, {"z", 0}})), (glm::ivec3{3, 2, 1}), (glm::ivec3{4, 2, 0}));
    AUTO_SUCCESS((Json::object({{"x", 1}, {"y", 2}})), (glm::ivec3{1, 2, 0}));
    AUTO_FAILURE((Json::object({{"w", 0}})), glm::ivec3);
    AUTO_FAILURE((Json::object({{"x", "0"}})), glm::ivec3);

    AUTO_EXISTING((Json::array({1, 2, 3})), (std::vector<int>{3, 2}), (std::vector<int>{1, 2, 3}));
    AUTO_EXISTING((Json::array({1, 2, 3})), (std::vector<int>{4, 3, 2, 1}), (std::vector<int>{1, 2, 3}));
    AUTO_SUCCESS((Json::array({1, 2, 3})), (std::vector<int>{1, 2, 3}));
    AUTO_FAILURE((Json::array({1, 2, "3"})), std::vector<int>);

    const auto& nullable = reflect<Nullable>();
    const auto& nullableTrait = nullable.get<NullableTrait>();
    Nullable null{144};
    AUTO_SUCCESS(144, null);

    nullableTrait.setToNull(&null);
    AUTO_SUCCESS(nlohmann::json::value_t::null, null);
}
// NOLINTEND(readability-function-size)
