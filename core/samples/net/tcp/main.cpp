#include <cstring>
#include <iostream>
#include <thread>

#include <cubos/core/log.hpp>
/// [Import headers]
#include <cubos/core/net/tcp_listener.hpp>
#include <cubos/core/net/tcp_stream.hpp>
/// [Import headers]
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using cubos::core::net::Address;
using cubos::core::net::TcpListener;
using cubos::core::net::TcpStream;

/// [Define constants]
#define SERVER_ADDRESS Address::LocalHost
#define SERVER_PORT 8080
/// [Define constants]

/// [Server function]
void runServer()
{
    TcpListener listener;
    listener.listen(SERVER_ADDRESS, SERVER_PORT, 1);

    CUBOS_DEBUG("Server is listening");

    TcpStream clientStream;
    while (listener.accept(clientStream))
    {
        char buffer[1024];
        if (std::size_t bytesRead = clientStream.read(buffer, sizeof(buffer)); bytesRead > 0)
        {
            CUBOS_INFO("Received message: {} with size {}", buffer, bytesRead);
            break;
        }
    }

    listener.close();
}
/// [Server function]

/// [Client function]
void runClient()
{
    TcpStream stream;
    stream.connect(SERVER_ADDRESS, SERVER_PORT);

    const char* message = "Hello Cubos!";
    stream.write(message, std::strlen(message));
    stream.disconnect();
}
/// [Client function]

int main()
{
    std::thread serverThread(runServer);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // wait so server is up
    std::thread clientThread(runClient);

    serverThread.join();
    clientThread.join();

    return 0;
}