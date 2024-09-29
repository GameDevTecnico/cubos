#include <latch>
#include <thread>

#include <doctest/doctest.h>
#include <glm/vec3.hpp>

#include <cubos/core/data/des/binary.hpp>
#include <cubos/core/data/ser/binary.hpp>
#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/net/tcp_listener.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/traits/fields.hpp>
#include <cubos/core/reflection/traits/mask.hpp>
#include <cubos/core/reflection/traits/nullable.hpp>
#include <cubos/core/reflection/type_client.hpp>
#include <cubos/core/reflection/type_server.hpp>

using cubos::core::data::BinaryDeserializer;
using cubos::core::data::BinarySerializer;
using cubos::core::memory::AnyValue;
using cubos::core::net::Address;
using cubos::core::net::TcpListener;
using cubos::core::net::TcpStream;
using cubos::core::reflection::ConstructibleTrait;
using cubos::core::reflection::EnumTrait;
using cubos::core::reflection::FieldsTrait;
using cubos::core::reflection::MaskTrait;
using cubos::core::reflection::NullableTrait;
using cubos::core::reflection::reflect;
using cubos::core::reflection::Type;
using cubos::core::reflection::TypeClient;
using cubos::core::reflection::TypeServer;

namespace
{
    enum class Color
    {
        Red,
        Green,
        Blue
    };

    enum class Permissions
    {
        None = 0,
        Read = 1,
        Write = 2,
        Execute = 4
    };

    inline Permissions operator~(Permissions mods)
    {
        return static_cast<Permissions>(~static_cast<int>(mods));
    }

    inline Permissions operator|(Permissions lhs, Permissions rhs)
    {
        return static_cast<Permissions>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline Permissions operator&(Permissions lhs, Permissions rhs)
    {
        return static_cast<Permissions>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    struct TestStruct
    {
        CUBOS_REFLECT;

        int i{1};
        bool b{true};
        glm::ivec3 v{-1, -2, -3};
        std::vector<std::unordered_map<int, std::string>> m{{}, {{1, "one"}, {2, "two"}}};
        uuids::uuid u = uuids::uuid::from_string("01234567-89ab-cdef-0123-456789abcdef").value();
        Color c{Color::Green};
        Permissions p{Permissions::Read | Permissions::Write};

        bool operator==(const TestStruct& other) const = default;
    };

    struct UnserializableStruct
    {
        CUBOS_REFLECT;
    };

    struct UnserializableBigStruct
    {
        CUBOS_REFLECT;

        int i;
        UnserializableStruct s;
    };
} // namespace

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, Color);
CUBOS_REFLECT_EXTERNAL_IMPL(Color)
{
    return Type::create("Color")
        .with(ConstructibleTrait::typed<Color>().withBasicConstructors().build())
        .with(EnumTrait{}.withVariant<Color::Red>("Red").withVariant<Color::Green>("Green").withVariant<Color::Blue>(
            "Blue"));
}

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, Permissions);
CUBOS_REFLECT_EXTERNAL_IMPL(Permissions)
{
    return Type::create("Permissions")
        .with(ConstructibleTrait::typed<Permissions>().withBasicConstructors().build())
        .with(MaskTrait{}
                  .withBit<Permissions::Read>("Read")
                  .withBit<Permissions::Write>("Write")
                  .withBit<Permissions::Execute>("Execute"));
}

CUBOS_REFLECT_IMPL(TestStruct)
{
    return Type::create("TestStruct")
        .with(ConstructibleTrait::typed<TestStruct>().withBasicConstructors().build())
        .with(FieldsTrait{}
                  .withField("i", &TestStruct::i)
                  .withField("b", &TestStruct::b)
                  .withField("v", &TestStruct::v)
                  .withField("m", &TestStruct::m)
                  .withField("u", &TestStruct::u)
                  .withField("c", &TestStruct::c)
                  .withField("p", &TestStruct::p))
        .with(NullableTrait{[](const void* value) { return !static_cast<const TestStruct*>(value)->b; },
                            [](void* value) { static_cast<TestStruct*>(value)->b = false; }});
}

CUBOS_REFLECT_IMPL(UnserializableStruct)
{
    return Type::create("UnserializableStruct")
        .with(ConstructibleTrait::typed<UnserializableStruct>().withBasicConstructors().build());
}

CUBOS_REFLECT_IMPL(UnserializableBigStruct)
{
    return Type::create("UnserializableBigStruct")
        .with(ConstructibleTrait::typed<UnserializableBigStruct>().withBasicConstructors().build())
        .with(FieldsTrait{}.withField("i", &UnserializableBigStruct::i).withField("s", &UnserializableBigStruct::s));
}

TEST_CASE("reflection::TypeClient & reflection::TypeServer")
{
    // You should probably uncomment this if you're debugging this test:
    cubos::core::tel::level(cubos::core::tel::Level::Trace);

    std::latch serverLatch{1};
    std::latch clientLatch{1};

    std::thread serverThread{[&]() {
        TcpListener listener{};
        REQUIRE(listener.listen(Address::LocalHost, 8888));
        serverLatch.count_down(); // Notify client that server is ready.

        TcpStream stream{};
        REQUIRE(listener.accept(stream)); // Wait for client to connect.
        BinarySerializer ser{stream};
        BinaryDeserializer des{stream};

        // Setup type server.
        TypeServer server{};
        server.addType<TestStruct>();
        server.addType<UnserializableBigStruct>();

        // Share types with client.
        auto connection = server.connect(stream);
        REQUIRE(connection.contains());

        REQUIRE(connection->sharedTypes().contains<TestStruct>());
        REQUIRE(connection->serializableTypes().contains<TestStruct>());
        CHECK(connection->sharedTypes().contains<UnserializableBigStruct>());
        CHECK_FALSE(connection->serializableTypes().contains<UnserializableBigStruct>());

        // Receive the default value of "TestStruct" from the client, check if it is correct.
        TestStruct received{};
        REQUIRE(des.read(received));
        CHECK(received == TestStruct{});

        // Send a value of "TestStruct", receive it back and check if it is correct.
        REQUIRE(ser.write(TestStruct{
            .i = 42,
            .b = true,
            .v = {1, 2, 3},
            .m = {{{1, "um"}, {2, "dois"}}},
            .u = uuids::uuid::from_string("00112233-4455-6677-8899-aabbccddeeff").value(),
            .c = Color::Blue,
            .p = Permissions::Read | Permissions::Execute,
        }));
        REQUIRE(des.read(received));
        CHECK(received.i == 42);
        CHECK(received.b == true);
        CHECK(received.v == glm::ivec3{1, 2, 3});
        CHECK(received.m == std::vector<std::unordered_map<int, std::string>>{{{1, "um"}, {2, "dois"}}});
        CHECK(received.u == uuids::uuid::from_string("00112233-4455-6677-8899-aabbccddeeff").value());
        CHECK(received.c == Color::Blue);
        CHECK(received.p == (Permissions::Read | Permissions::Execute));

        clientLatch.wait(); // Wait for client to disconnect.
    }};

    TcpStream stream{};
    serverLatch.wait(); // Wait for server to be ready.
    REQUIRE(stream.connect(Address::LocalHost, 8888));
    BinarySerializer ser{stream};
    BinaryDeserializer des{stream};

    // Setup type client.
    TypeClient client{};
    auto connection = client.connect(stream);
    REQUIRE(connection.contains());

    REQUIRE(connection->sharedTypes().contains("TestStruct"));
    REQUIRE(connection->sharedTypes().contains("Color"));
    REQUIRE(connection->sharedTypes().contains("Permissions"));
    CHECK(connection->sharedTypes().contains("UnserializableBigStruct"));
    CHECK(connection->sharedTypes().contains("UnserializableStruct"));

    REQUIRE(connection->serializableTypes().contains("TestStruct"));
    REQUIRE(connection->serializableTypes().contains("Color"));
    REQUIRE(connection->serializableTypes().contains("Permissions"));
    CHECK_FALSE(connection->serializableTypes().contains("UnserializableBigStruct"));
    CHECK_FALSE(connection->serializableTypes().contains("UnserializableStruct"));

    {
        // Send a default "TestStruct" value to the server.
        auto testStruct = AnyValue::defaultConstruct(connection->serializableTypes().at("TestStruct"));
        REQUIRE(ser.write(testStruct.type(), testStruct.get()));

        // Receive a "TestStruct" value from the server and send it back.
        REQUIRE(des.read(testStruct.type(), testStruct.get()));
        REQUIRE(ser.write(testStruct.type(), testStruct.get()));
    } // Make sure we don't hold references to connection types after disconnect.

    stream.disconnect();
    clientLatch.count_down(); // Notify server that client is disconnected.
    serverThread.join();
}
