#include <cstring>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
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
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::net::UdpSocket;

UdpSocket::UdpSocket()
{
#ifdef _WIN32
    WSADATA wsa;
    CUBOS_ASSERT(WSAStartup(MAKEWORD(2, 2), &wsa) == 0, " WSAStartup failed: {}", WSAGetLastError());
#endif

    mSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (mSock == InnerInvalidSocket)
    {
        CUBOS_ERROR("Failed to allocate UDP socket: {}", logSystemError());
        return;
    }

    CUBOS_TRACE("New UDP socket: '{}'", mSock);
}

UdpSocket::~UdpSocket()
{
    unbind();
}

bool UdpSocket::bind(uint16_t port, const Address& address) const
{
    if (mSock == InnerInvalidSocket)
    {
        CUBOS_ERROR("Invalid socket: {}", logSystemError());
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.toString().c_str());

    if (::bind(mSock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1)
    {
        CUBOS_ERROR("Failed to bind UDP socket: {}", logSystemError());
        return false;
    }

    CUBOS_DEBUG("Bind UDP socket '{}' at address {} with port '{}'", mSock, address.toString(), port);
    return true;
}

void UdpSocket::unbind()
{
    if (mSock != InnerInvalidSocket)
    {
        CUBOS_WARN("Closing UDP socket: '{}'", mSock);
#ifdef _WIN32
        closesocket(mSock);
        WSACleanup();
#else
        close(mSock);
        mSock = InnerInvalidSocket;
#endif
    }
}

bool UdpSocket::send(const void* data, std::size_t size, const Address& address, uint16_t port) const
{
    if (mSock == InnerInvalidSocket)
    {
        CUBOS_ERROR("Invalid socket: {}", logSystemError());
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.toString().c_str());

#ifdef _WIN32
    auto res = sendto(mSock, static_cast<const char*>(data), static_cast<int>(size), 0,
                      reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
#else
    auto res = sendto(mSock, data, size, 0, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
#endif
    if (res == InnerInvalidSocket)
    {
        CUBOS_ERROR("Failed to send UDP data: {}", logSystemError());
        return false;
    }

    CUBOS_TRACE("Sent UDP message to socket '{}' at address {} and port '{}'", mSock, address.toString(), port);
    return true;
}

bool UdpSocket::receive(void* data, std::size_t size, std::size_t& received, Address& address) const
{
    if (mSock == InnerInvalidSocket)
    {
        CUBOS_ERROR("Invalid socket: {}", logSystemError());
        return false;
    }

    sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);

#ifdef _WIN32
    auto len = recvfrom(mSock, static_cast<char*>(data), static_cast<int>(size), 0,
                        reinterpret_cast<struct sockaddr*>(&addr), &addrLen);
#else
    auto len = recvfrom(mSock, data, size, 0, reinterpret_cast<struct sockaddr*>(&addr), &addrLen);
#endif

    if (len == InnerInvalidSocket)
    {
        CUBOS_ERROR("Failed to receive UDP data: {}", logSystemError());
        return false;
    }

    received = static_cast<std::size_t>(len);
    address = Address::from(std::string(inet_ntoa(addr.sin_addr))).value();

    CUBOS_TRACE("Received UDP message at socket '{}' from address '{}'", mSock, address.toString());
    return true;
}

bool UdpSocket::setBlocking(bool blocking)
{
    bool success = false;

#ifdef _WIN32
    u_long flags = blocking ? 0 : 1;
    success = NO_ERROR == ioctlsocket(mSock, FIONBIO, &flags);
#else
    mBlocking = blocking;

    int flags = fcntl(mSock, F_GETFL, 0);

    if (blocking)
    {
        flags &= ~O_NONBLOCK;
    }
    else
    {
        flags |= O_NONBLOCK;
    }

    success = fcntl(mSock, F_SETFL, flags) == 0;
#endif
    if (!success)
    {
        CUBOS_ERROR("Failed to change block flag of socket '{}'", mSock);
    }

    return success;
}
