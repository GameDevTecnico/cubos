/// @file
/// @brief Class @ref cubos::core::net::UdpSocket.
/// @ingroup core-net

#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <cubos/core/api.hpp>
#include <cubos/core/net/address.hpp>

#ifdef _WIN32
#include <winsock2.h>
// Receive and Send buffer types. Differs from Windows (int) and POSIX (ssize_t/size_t).
#define BUF_SEND_TYPE int
#define BUF_RECEIVE_TYPE int
#include <winSock2.h>
#else
// Receive and Send buffer types. Differs from Windows (int) and POSIX (ssize_t/size_t).
#define BUF_SEND_TYPE size_t
#define BUF_RECEIVE_TYPE ssize_t
// Socket type. Differs from Windows (SOCKET) and POSIX (int).
#define SOCKET int
// Reprensets an invalid socket.
#define INVALID_SOCKET (-1)
#endif

namespace cubos::core::net
{
    /// @brief Represents a UDP socket.
    /// @ingroup core-net
    class CUBOS_CORE_API UdpSocket
    {
    public:
        /// @brief Constructs a empty UDP socket.
        UdpSocket();

        /// @brief Deconstructs by unbinding the inner socket.
        ~UdpSocket();

        /// @brief Forbid copy construction.
        UdpSocket(const UdpSocket&) = delete;

        /// @brief Forbid copy assignment.
        UdpSocket& operator=(const UdpSocket&) = delete;

        /// @brief Creates a UDP socket from the given `port` and `address`.
        /// @param port Port to bind.
        /// @param address Address to bind.
        /// @return Whether binding was successful.
        bool bind(uint16_t port, const Address& address = Address::Any);

        /// @brief Closes the inner socket.
        void unbind() const;

        /// @brief Sends `data` on the socket to the remote `address` and `port`.
        /// @param data Message.
        /// @param size Message size.
        /// @param address Address to send.
        /// @param port Port to send.
        /// @return Whether data was sent sucessfully.
        bool send(const char* data, BUF_SEND_TYPE size, const Address& address, uint16_t port) const;

        /// @brief Receives a single datagram message on the socket.
        /// @param data Buffer to store the datagram message.
        /// @param size Maximum buffer size.
        /// @param[out] received Number of bytes read.
        /// @param[out] address Origin address.
        /// @return Whether data was received sucessfully.
        bool receive(char* data, BUF_SEND_TYPE size, BUF_RECEIVE_TYPE& received, Address& address) const;

        /// @brief Control blocking mode of inner socket.
        /// @param blocking Whether to block or not.
        /// @note A blocking socket does not return control until it has sent (or received) some or all data specified
        /// for the operation
        /// @note A non-blocking socket returns whatever is in the receive buffer and immediately continues.
        void setBlocking(bool blocking);

    private:
        SOCKET mSock{INVALID_SOCKET};
        bool mBlocking{true};
    };
} // namespace cubos::core::net