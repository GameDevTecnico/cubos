#include <chrono>
#include <cstring>
#include <thread>

#include <doctest/doctest.h>
#include <glm/glm.hpp>

#include <cubos/core/log.hpp>
#include <cubos/core/net/udp_socket.hpp>

TEST_CASE("udp socket")
{
    using cubos::core::net::Address;
    using cubos::core::net::UdpSocket;

    SUBCASE("bind")
    {
        UdpSocket socket;
        REQUIRE(socket.bind(8080));
    }

    SUBCASE("double bind")
    {
        UdpSocket socket1;
        UdpSocket socket2;
        REQUIRE(socket1.bind(8080));
        REQUIRE(!socket2.bind(8080));
    }

    SUBCASE("send and receive")
    {
        UdpSocket socket1, socket2;
        REQUIRE(socket1.bind(8080));
        REQUIRE(socket2.bind(8081));

        const char* msg = "hi";
        auto len = static_cast<BUF_SEND_TYPE>(strlen(msg));
        Address sendAddress = Address::fromIPv4(127, 0, 0, 1);
        REQUIRE(socket1.send(msg, len, sendAddress, 8081));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        char buffer[1024];
        BUF_RECEIVE_TYPE receivedSize;
        Address recvAddress;
        REQUIRE(socket2.receive(buffer, sizeof(buffer), receivedSize, recvAddress));
        REQUIRE(receivedSize == len);
        REQUIRE(*buffer == *msg);
    }

#ifndef _WIN32
    SUBCASE("blocking")
    {
        UdpSocket socket;
        REQUIRE(socket.bind(8080));

        socket.setBlocking(false);

        char buffer[1024];
        BUF_RECEIVE_TYPE receivedSize;
        Address recvAddress;
        REQUIRE(!socket.receive(buffer, sizeof(buffer), receivedSize, recvAddress));

        socket.setBlocking(true);
        // ???
    }
#endif

    SUBCASE("send and receive without binding")
    {
        UdpSocket socket;
        Address addr;
        BUF_RECEIVE_TYPE receivedSize;
        REQUIRE(!socket.send(nullptr, 0, addr, 0));
        REQUIRE(!socket.receive(nullptr, 0, receivedSize, addr));
    }
}
