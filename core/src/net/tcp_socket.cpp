#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cubos/core/log.hpp>
#include <cubos/core/net/tcp_socket.hpp>

using cubos::core::net::Address;
using cubos::core::net::TcpSocket;

TcpSocket::TcpSocket()
    : mSock(-1)
    , mBlocking(true)
{
}

TcpSocket::~TcpSocket()
{
}

TcpSocket::ResultCode TcpSocket::connect(const Address& address, uint16_t port, int timeoutMs)
{
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(address.toString().c_str());
    addr.sin_port = htons(port);

    if (::connect(mSock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        CUBOS_ERROR("Could not connect to address {} at port '{}'", address.toString(), port);
        return ResultCode::ConnectError;
    }

    (void)timeoutMs;
    // TODO implement timeout

    CUBOS_INFO("New TCP connection at address {} and port '{}'", address.toString(), port);
    return ResultCode::Success;
}

TcpSocket::ResultCode TcpSocket::send(const void* data, size_t size)
{
    if (::send(mSock, data, size, 0) < 0)
    {
        CUBOS_ERROR("Could not send TCP message with socket '{}'", mSock);
        return ResultCode::SendError;
    }

    CUBOS_INFO("Sent TCP message");
    return ResultCode::Success;
}

TcpSocket::ResultCode TcpSocket::receive(void* data, size_t size, ssize_t& received)
{
    received = ::recv(mSock, data, size, 0);
    if (received < 0)
    {
        CUBOS_ERROR("Could not receive TCP message with socket '{}'", mSock);
        return ResultCode::ReceiveError;
    }

    CUBOS_INFO("Received TCP message");
    return ResultCode::Success;
}

void TcpSocket::setBlocking(bool blocking)
{
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
    CUBOS_INFO("TCP socket '{}' is now in '{}' mode", mSock, blocking ? "blocking" : "non-blocking");
}
