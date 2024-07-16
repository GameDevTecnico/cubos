#include <cstring>
#include <thread>

#include <doctest/doctest.h>

#include <cubos/core/net/address.hpp>
#include <cubos/core/net/tcp_listener.hpp>
#include <cubos/core/net/tcp_stream.hpp>

TEST_CASE("tcp")
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

        std::thread serverThread{[&svAddr, &svPort]() {
            TcpListener listener;
            REQUIRE(listener.listen(svAddr, svPort, 1));

            TcpStream stream;
            REQUIRE(listener.accept(stream));

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

        std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // wait so server is up

        TcpStream stream;
        REQUIRE(stream.connect(svAddr, svPort));

        const char* msg = "ping";
        REQUIRE(stream.write(msg, 4));

        char readBuffer[5];
        REQUIRE(stream.peek() == 'p');
        REQUIRE(stream.peek() == 'p');
        REQUIRE(stream.readExact(readBuffer, 4));
        readBuffer[4] = '\0';
        REQUIRE(std::string(readBuffer) == "pong");

        stream.disconnect();
        REQUIRE(!stream.valid());

        serverThread.join();
    }

    SUBCASE("empty tcp stream")
    {
        TcpStream stream;
        REQUIRE(!stream.valid());
        REQUIRE(!stream.eof());
    }

    SUBCASE("inner valid")
    {
        TcpStream stream;
        stream.inner(1337);
        REQUIRE(stream.valid());
    }

    SUBCASE("stream inner invalid")
    {
        TcpStream stream;
        stream.inner(InnerInvalidSocket);
        REQUIRE(!stream.valid());
    }

    SUBCASE("stream disconnect")
    {
        TcpStream stream;
        stream.inner(1336); // dummy value, not a real socket
        REQUIRE(stream.valid());

        stream.disconnect();
        REQUIRE(!stream.valid());
    }

    SUBCASE("listener disconnect")
    {
        TcpListener listener;
        REQUIRE(!listener.valid());
    }
}