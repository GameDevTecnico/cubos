#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <cstring>
#include <memory>

#include <cubos/core/log.hpp>
#include <cubos/core/net/tcp_listener.hpp>
#include <cubos/core/net/tcp_stream.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::net::Address;
using cubos::core::net::TcpListener;
using cubos::core::net::TcpStream;

TcpListener::TcpListener() // NOLINT(modernize-use-equals-default)
{
#ifdef _WIN32
    WSADATA wsa;
    CUBOS_ASSERT(WSAStartup(MAKEWORD(2, 2), &wsa) == 0, " WSAStartup failed: {}", WSAGetLastError());
#endif
}

TcpListener::~TcpListener()
{
    close();
}

bool TcpListener::listen(const Address& address, uint16_t port, int maxQueueLength)
{
    this->create();

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.toString().c_str());

    if (::bind(mSock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1)
    {
        CUBOS_ERROR("Failed to bind TCP socket: {}", logSystemError());
        return false;
    }

    if (::listen(mSock, maxQueueLength) < 0)
    {
        CUBOS_ERROR("Failed to listen to TCP socket: {}", logSystemError());
        return false;
    }

    CUBOS_DEBUG("TCP listener socket '{}' bound to address {} with port '{}'", mSock, address.toString(), port);
    return true;
}

bool TcpListener::accept(TcpStream& stream) const
{
    if (mSock == InnerInvalidSocket)
    {
        CUBOS_ERROR("Listener hasn't been initialized correctly");
        return false;
    }

    auto clientSocket = ::accept(mSock, nullptr, nullptr);
    if (clientSocket == InnerInvalidSocket)
    {
        CUBOS_ERROR("Failed to accept TCP connection: {}", logSystemError());
        return false;
    }

    stream.inner(clientSocket);
    return true;
}

void TcpListener::close()
{
    if (mSock != InnerInvalidSocket)
    {
#ifdef _WIN32
        closesocket(mSock);
        WSACleanup();
#else
        ::close(mSock);
#endif
        mSock = InnerInvalidSocket;
    }
}

bool TcpListener::setBlocking(bool blocking)
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

bool TcpListener::create()
{
    this->close();

    mSock = socket(AF_INET, SOCK_STREAM, 0);
    if (mSock == InnerInvalidSocket)
    {
        CUBOS_ERROR("Failed to create TCP socket, got error {}", logSystemError());
        return false;
    }

    this->setBlocking(mBlocking);
    return true;
}