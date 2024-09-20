#include <cstring>
#include <latch>
#include <thread>

#include <doctest/doctest.h>

#include <cubos/core/net/address.hpp>
#include <cubos/core/net/tcp_listener.hpp>
#include <cubos/core/net/tcp_stream.hpp>

TEST_CASE("net::Tcp*")
{
    using cubos::core::net::Address;
    using cubos::core::net::InnerInvalidSocket;
    using cubos::core::net::InnerSocket;
    using cubos::core::net::TcpListener;
    using cubos::core::net::TcpStream;

    SUBCASE("write and read with stream and listener")
    {
        auto svAddr = Address::fromIPv4(127, 0, 0, 1);
        uint16_t svPort = 8080;
        std::latch svLatch{1};

        std::jthread serverThread{[&svAddr, &svPort, &svLatch]() {
            TcpListener listener;
            REQUIRE(listener.listen(svAddr, svPort, 1));
            REQUIRE(listener.valid());
            svLatch.count_down();

            TcpStream stream;
            REQUIRE(listener.accept(stream));
            REQUIRE(stream.valid());

            char readBuffer[5]; // 4 bytes for "ping" and 1 for the null terminator
            REQUIRE(stream.read(readBuffer, 4));
            readBuffer[4] = '\0';
            REQUIRE(std::string(readBuffer) == "ping");

            REQUIRE(stream.write("pong", 4));

            stream.disconnect();
            listener.close();
            REQUIRE(!stream.valid());
            REQUIRE(!listener.valid());
        }};

        TcpStream stream;
        svLatch.wait(); // wait for the server to start
        REQUIRE(stream.connect(svAddr, svPort));
        REQUIRE(stream.valid());

        const char* msg = "ping";
        REQUIRE(stream.write(msg, 4));

        char readBuffer[5];
        REQUIRE(stream.peek() == 'p');
        REQUIRE(stream.peek() == 'p');
        REQUIRE(stream.readExact(readBuffer, 4));
        readBuffer[4] = '\0';
        REQUIRE(std::string(readBuffer) == "pong");

        stream.disconnect();
        REQUIRE_FALSE(stream.valid());
    }

    SUBCASE("empty tcp stream")
    {
        TcpStream stream;
        REQUIRE_FALSE(stream.valid());
        REQUIRE_FALSE(stream.eof());
    }

    SUBCASE("stream inner invalid")
    {
        TcpStream stream;
        REQUIRE_FALSE(stream.valid());
        stream.inner(InnerInvalidSocket);
        REQUIRE_FALSE(stream.valid());
    }

    SUBCASE("stream disconnect")
    {
        TcpStream stream;
        REQUIRE_FALSE(stream.valid());
        stream.disconnect();
        REQUIRE_FALSE(stream.valid());
    }

    SUBCASE("listener disconnect")
    {
        TcpListener listener;
        REQUIRE_FALSE(listener.valid());
        listener.close();
        REQUIRE_FALSE(listener.valid());
    }
}