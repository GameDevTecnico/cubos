#include <cstring>
#include <iostream>
#include <thread>

#include <cubos/core/tel/logging.hpp>
/// [Import header]
#include <cubos/core/net/udp_socket.hpp>
/// [Import header]
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::net::Address;
using cubos::core::net::UdpSocket;

/// [Define constants]
#define SERVER_ADDRESS Address::LocalHost
#define SERVER_PORT 8080

#define CLIENT_ADDRESS Address::LocalHost
#define CLIENT_PORT 8081
/// [Define constants]

/// [Server function]
void runServer()
{
    UdpSocket server;
    CUBOS_INFO("Server is running and waiting for messages at port {}", SERVER_PORT);

    char buffer[1024];
    size_t received;
    Address senderAddress;
    while (true)
    {
        if (server.receive(buffer, sizeof(buffer), received, senderAddress))
        {
            buffer[received] = '\0'; // C strings :)
            CUBOS_INFO("Received message: {} with size {}", buffer, received);
            break;
        }

        CUBOS_ERROR("Receive error");
    }
}
/// [Server function]

/// [Client function]
void runClient()
{
    UdpSocket client;
    CUBOS_ASSERT(client.bind(CLIENT_PORT, CLIENT_ADDRESS), "Failed to bind client socket");

    // sleep for a bit to make sure server is up before sending message
    std::this_thread::sleep_for(std::chrono::seconds(1));

    const char* msg = "Hello, I'm a Cubos UDP client!";
    if (!client.send(msg, std::strlen(msg), SERVER_ADDRESS, SERVER_PORT))
    {
        CUBOS_ERROR("Failed to send message");
        return;
    }

    CUBOS_INFO("Message sent to server");
}
/// [Client function]

/// [Main function]
int main()
{
    std::thread serverThread(runServer);
    std::thread clientThread(runClient);

    serverThread.join();
    clientThread.join();

    return 0;
}
/// [Main function]