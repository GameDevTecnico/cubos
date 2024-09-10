#include <doctest/doctest.h>
#include <glm/vec3.hpp>

#include <cubos/core/data/des/binary.hpp>
#include <cubos/core/data/ser/binary.hpp>
#include <cubos/core/memory/buffer_stream.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/enum.hpp>

#include "../utils.hpp"

using cubos::core::data::BinaryDeserializer;
using cubos::core::data::BinarySerializer;
using cubos::core::data::Serializer;
using cubos::core::memory::BufferStream;
using cubos::core::memory::SeekOrigin;
using cubos::core::reflection::Type;

#define AUTO_TEST(type, value)                                                                                         \
    {                                                                                                                  \
        type result{};                                                                                                 \
        stream.seek(0, SeekOrigin::Begin);                                                                             \
        CHECK(ser.write<type>(value));                                                                                 \
        stream.seek(0, SeekOrigin::Begin);                                                                             \
        CHECK(des.read<type>(result));                                                                                 \
        CHECK_EQ(value, result);                                                                                       \
    }

#define AUTO_TEST_IMPL(...) AUTO_TEST(decltype(__VA_ARGS__), (__VA_ARGS__))

namespace
{
    enum class Color
    {
        Red,
        Green,
        Blue
    };
} // namespace

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, Color);
CUBOS_REFLECT_EXTERNAL_IMPL(Color)
{
    return Type::create("Color").with(
        EnumTrait{}.withVariant<Color::Red>("Red").withVariant<Color::Green>("Green").withVariant<Color::Blue>("Blue"));
}

TEST_CASE("data::Binary(De)Serializer")
{
    BufferStream stream{};
    BinarySerializer ser{stream};
    BinaryDeserializer des{stream};

    AUTO_TEST(bool, false);
    AUTO_TEST(bool, true);
    AUTO_TEST(char, 'a');
    AUTO_TEST(int8_t, -120);
    AUTO_TEST(int16_t, 30000);
    AUTO_TEST(int32_t, -2000000000);
    AUTO_TEST(int64_t, 9000000000000000000);
    AUTO_TEST(uint8_t, 120);
    AUTO_TEST(uint16_t, 60000);
    AUTO_TEST(uint32_t, 4000000000);
    AUTO_TEST(uint64_t, 18000000000000000000ULL);
    AUTO_TEST(float, 1.5F);
    AUTO_TEST(double, 1.0);
    AUTO_TEST(std::string, "Hello, world!");
    AUTO_TEST(uuids::uuid, *uuids::uuid::from_string("f7063cd4-de44-47b5-b0a9-f0e7a558c9e5"));

    AUTO_TEST_IMPL(std::map<std::string, bool>{{"false", false}, {"true", true}});
    AUTO_TEST_IMPL(std::vector<int>{1, 2, 3});
    AUTO_TEST_IMPL(glm::tvec3<int>{1, 2, 3});
    AUTO_TEST_IMPL(Color::Red);
    AUTO_TEST_IMPL(Color::Green);
    AUTO_TEST_IMPL(Color::Blue);
}
