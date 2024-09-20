#include <chrono>
#include <cstring>
#include <thread>

#include <doctest/doctest.h>
#include <glm/glm.hpp>

#include <cubos/core/log.hpp>
#include <cubos/core/net/udp_socket.hpp>

TEST_CASE("net::UdpSocket")
{
    using cubos::core::net::Address;
    using cubos::core::net::UdpSocket;

    SUBCASE("bind")
    {
        UdpSocket socket;
        REQUIRE(socket.bind(8080));
    }

    SUBCASE("double bind same port")
    {
        UdpSocket socket1;
        UdpSocket socket2;
        REQUIRE(socket1.bind(8080));
        REQUIRE(!socket2.bind(8080));
    }

    SUBCASE("double bind same socket")
    {
        UdpSocket socket;
        REQUIRE(socket.bind(8080));
        REQUIRE(!socket.bind(8080));
    }

    SUBCASE("send and receive")
    {
        UdpSocket socket1, socket2;
        REQUIRE(socket1.bind(8080));
        REQUIRE(socket2.bind(8081));

        const char* msg = "hi";
        std::size_t len = std::strlen(msg);
        Address sendAddress = Address::fromIPv4(127, 0, 0, 1);
        REQUIRE(socket1.send(msg, len, sendAddress, 8081));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        char buffer[1024];
        std::size_t receivedSize;
        Address recvAddress;
        REQUIRE(socket2.receive(buffer, sizeof(buffer), receivedSize, recvAddress));
        REQUIRE(receivedSize == len);
        REQUIRE(recvAddress.toString() == sendAddress.toString());
        REQUIRE(*buffer == *msg);
    }

    SUBCASE("blocking")
    {
        UdpSocket socket;
        REQUIRE(socket.setBlocking(false));

        char buffer[1024];
        std::size_t receivedSize;
        Address recvAddress;
        REQUIRE(!socket.receive(buffer, sizeof(buffer), receivedSize, recvAddress));
    }

    SUBCASE("send without binding")
    {
        UdpSocket socket;
        Address addr = Address::LocalHost;
        REQUIRE(socket.send("ping", 4, addr, 8080));
    }
}
