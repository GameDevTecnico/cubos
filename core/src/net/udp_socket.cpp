#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <cubos/core/log.hpp>
#include <cubos/core/net/udp_socket.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::net::UdpSocket;

UdpSocket::UdpSocket()
{
#ifdef _WIN32
    WSADATA wsa;
    CUBOS_ASSERT(WSAStartup(MAKEWORD(2, 2), &wsa) == 0, " WSAStartup failed: {}", WSAGetLastError());
#endif
}

UdpSocket::~UdpSocket()
{
    unbind();
}

/// @brief Cross-platform utility to get last error message.
/// @return Message.
/// @todo Move this to a higher module for general purposes?
static std::string error()
{
#ifdef _WIN32
    static char message[256] = {0};
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, WSAGetLastError(), 0, message, 256, 0);
    char* nl = strrchr(message, '\n');
    if (nl)
        *nl = 0;
    return {message};
#else
    return {strerror(errno)};
#endif
}

bool UdpSocket::bind(uint16_t port, const Address& address)
{
    mSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSock == -1)
    {
        CUBOS_ERROR("Failed to allocate UDP socket: {}", error());
        return false;
    }

    CUBOS_TRACE("New UDP socket: '{}'", mSock);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.toString().c_str());

    if (::bind(mSock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1)
    {
        CUBOS_ERROR("Failed to bind UDP socket: {}", error());
        unbind();
        return false;
    }

    CUBOS_INFO("Bind UDP socket '{}' at address {} with port '{}'", mSock, address.toString(), port);
    return true;
}

void UdpSocket::unbind() const
{
    if (mSock != -1)
    {
        CUBOS_WARN("Closing UDP socket: '{}'", mSock);
#ifdef _WIN32
        closesocket(mSock);
        WSACleanup();
#else
        close(mSock);
#endif
    }
}

bool UdpSocket::send(const char* data, BUF_SEND_TYPE size, const Address& address, uint16_t port) const
{
    if (mSock == -1)
    {
        CUBOS_ERROR("Invalid socket: {}", error());
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.toString().c_str());

    if (sendto(mSock, data, size, 0, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1)
    {
        CUBOS_ERROR("Failed to send UDP data: {}", error());
        return false;
    }

    CUBOS_INFO("Sent UDP message to socket '{}' at address {} and port '{}'", mSock, address.toString(), port);
    return true;
}

bool UdpSocket::receive(char* data, BUF_SEND_TYPE size, BUF_RECEIVE_TYPE& received, Address& address) const
{
    if (mSock == -1)
    {
        CUBOS_ERROR("Invalid socket: {}", error());
        return false;
    }

    sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);

    auto len = recvfrom(mSock, data, size, 0, reinterpret_cast<struct sockaddr*>(&addr), &addrLen);
    if (len == -1)
    {
        CUBOS_ERROR("Failed to receive UDP data: {}", error());
        return false;
    }

    received = len;
    address = Address::from(std::string(inet_ntoa(addr.sin_addr))).value();
    data[len] = '\0';

    CUBOS_INFO("Received UDP message at socket '{}' from address '{}'", mSock, address.toString());
    return true;
}

void UdpSocket::setBlocking(bool blocking)
{
#ifdef _WIN32
    (void)blocking;
    CUBOS_FAIL("OS not supported");
#else
    mBlocking = blocking;

    int flags = fcntl(mSock, F_GETFL, 0);
    if (flags == -1)
    {
        return;
    }

    if (blocking)
    {
        flags &= ~O_NONBLOCK;
    }
    else
    {
        flags |= O_NONBLOCK;
    }

    fcntl(mSock, F_SETFL, flags);
    CUBOS_INFO("Socket {} is now '{}'", mSock, blocking ? "blocking" : "non-blocking");
#endif
}
