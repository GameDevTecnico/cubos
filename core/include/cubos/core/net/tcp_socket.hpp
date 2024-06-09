/// @file
/// @brief Class @ref cubos::core::net::TcpSocket.
/// @ingroup core-net

#pragma once

#include <cstdint>

#include <cubos/core/api.hpp>
#include <cubos/core/net/address.hpp>

namespace cubos::core::net
{
    /// @brief Represents a TCP socket.
    /// @ingroup core-net
    class CUBOS_CORE_API TcpSocket
    {
    public:
        /// @brief Represents a result code.
        enum class ResultCode
        {
            Success,
            ConnectError,
            SendError,
            ReceiveError,
            UnknownError
        };

        /// @brief Constructs a empty TCP socket.
        TcpSocket();

        /// @brief Deconstructs by desconnecting the inner socket.
        ~TcpSocket();

        /// @brief Forbid copy construction.
        TcpSocket(const TcpSocket&) = delete;

        /// @brief Forbid copy assignment.
        TcpSocket& operator=(const TcpSocket&) = delete;

        /// @brief Connects via TCP to a remote `address` and `port`.
        /// @param address Destination address.
        /// @param port Destination port.
        /// @param timeoutMs Connection timeout in milliseconds.
        /// @return Result code.
        ResultCode connect(const Address& address, uint16_t port, int timeoutMs = 0);

        /// @brief Sends `data` to the connected socket.
        /// @param data Message.
        /// @param size Message size.
        /// @return Result code.
        ResultCode send(const void* data, size_t size);

        /// @brief Receives a single datagram message on the socket.
        /// @param data Buffer to store the datagram message.
        /// @param size Maximum buffer size.
        /// @param[out] received Number of bytes read.
        /// @return Result code.
        ResultCode receive(void* data, size_t size, ssize_t& received);

        /// @brief Control `blocking` mode of inner socket.
        /// @param blocking Whether to block or not.
        /// @note A blocking socket does not return control until it has sent (or received) some or all data specified
        /// for the operation
        /// @note A non-blocking socket returns whatever is in the receive buffer and immediately continues.
        void setBlocking(bool blocking);

    private:
        int mSock;
        bool mBlocking;
    };
} // namespace cubos::core::net