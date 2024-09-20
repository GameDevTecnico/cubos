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

#include <cubos/core/log.hpp>
#include <cubos/core/net/tcp_stream.hpp>
#include <cubos/core/net/utils.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using cubos::core::net::Address;
using cubos::core::net::TcpStream;

TcpStream::TcpStream()
{
#ifdef _WIN32
    WSADATA wsa;
    CUBOS_ASSERT(WSAStartup(MAKEWORD(2, 2), &wsa) == 0, " WSAStartup failed: {}", WSAGetLastError());
#endif
}

TcpStream::~TcpStream()
{
    disconnect();
}

TcpStream::TcpStream(TcpStream&& other) noexcept
    : mSock(other.mSock)
    , mBlocking(other.mBlocking)
    , mEof(other.mEof)
{
    other.mSock = InnerInvalidSocket;
}

bool TcpStream::connect(const Address& address, uint16_t port)
{
    if (!this->create())
    {
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(address.toString().c_str());
    addr.sin_port = htons(port);

    if (::connect(mSock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        CUBOS_ERROR("Failed to connect TCP stream to {}:{}: {}", address.toString(), port, logSystemError());
        return false;
    }

    CUBOS_DEBUG("TCP stream connected to address {} and port '{}' successfully", address.toString(), port);
    return true;
}

void TcpStream::inner(InnerSocket socket)
{
    this->disconnect();
    mSock = socket;
}

void TcpStream::disconnect()
{
    if (mSock != InnerInvalidSocket)
    {
#ifdef _WIN32
        closesocket(mSock);
        WSACleanup();
#else
        close(mSock);
#endif
        mSock = InnerInvalidSocket;
    }
}

std::size_t TcpStream::read(void* buffer, std::size_t size)
{
    if (mSock == InnerInvalidSocket)
    {
        CUBOS_ERROR("Invalid socket");
        return 0;
    }

#ifdef _WIN32
    auto bytesRead = ::recv(mSock, static_cast<char*>(buffer), static_cast<int>(size), 0);
#else
    auto bytesRead = ::recv(mSock, static_cast<char*>(buffer), size, 0);
#endif
    if (bytesRead < 0)
    {
        CUBOS_ERROR("Failed to read from TCP stream: {}", logSystemError());
        return 0;
    }

    if (bytesRead == 0)
    {
        mEof = true;
    }

    auto bytesReadCasted = static_cast<std::size_t>(bytesRead);
    return bytesReadCasted;
}

std::size_t TcpStream::write(const void* buffer, std::size_t size)
{
    if (mSock == InnerInvalidSocket)
    {
        CUBOS_ERROR("Invalid socket");
        return 0;
    }

#ifdef _WIN32
    auto bytesWritten = ::send(mSock, static_cast<const char*>(buffer), static_cast<int>(size), 0);
#else
    auto bytesWritten = ::send(mSock, buffer, size, 0);
#endif
    if (bytesWritten < 0)
    {
        CUBOS_ERROR("Failed to write TCP buffer: {}", logSystemError());
        return 0;
    }

    auto bytesWrittenCasted = static_cast<std::size_t>(bytesWritten);
    return bytesWrittenCasted;
}

std::size_t TcpStream::tell() const
{
    CUBOS_FAIL("Not implemented");
}

char TcpStream::peek()
{
    if (mSock == InnerInvalidSocket)
    {
        CUBOS_ERROR("Invalid socket");
        return '\0';
    }

    char c;

#ifdef _WIN32
    auto bytesRead = ::recv(mSock, &c, 1, MSG_PEEK);
#else
    auto bytesRead = ::recv(mSock, static_cast<void*>(&c), 1, MSG_PEEK);
#endif
    if (bytesRead < 0)
    {
        CUBOS_ERROR("Failed to peek from TCP stream: {}", logSystemError());
        return '\0';
    }

    if (bytesRead == 0)
    {
        CUBOS_WARN("TCP stream EOF");
        mEof = true;
        return '\0';
    }

    return c;
}

void TcpStream::seek(ptrdiff_t /*offset*/, cubos::core::memory::SeekOrigin /*origin*/)
{
    CUBOS_FAIL("Not implemented");
}

bool TcpStream::setBlocking(bool blocking)
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

bool TcpStream::create()
{
    this->disconnect();

    mSock = socket(AF_INET, SOCK_STREAM, 0);
    if (mSock == InnerInvalidSocket)
    {
        CUBOS_ERROR("Failed to create TCP socket, got error {}", logSystemError());
        return false;
    }
    else
    {
        this->setBlocking(mBlocking);
        return true;
    }
}
