/// @file
/// @brief Class @ref cubos::core::net::TcpListener.
/// @ingroup core-net

#pragma once

#include <cubos/core/api.hpp>
#include <cubos/core/net/address.hpp>
#include <cubos/core/net/tcp_stream.hpp>
#include <cubos/core/net/utils.hpp>

namespace cubos::core::net
{
    /// @brief A TCP socket server, listening for connections.
    /// @ingroup core-net
    class CUBOS_CORE_API TcpListener final
    {
    public:
        /// @brief Create an empty TCP listener, ready for connecting.
        TcpListener();

        /// @brief Deconstructs by calling @ref close.
        ~TcpListener();

        /// @brief Forbid copy construction.
        TcpListener(const TcpListener&) = delete;

        /// @brief Forbid assignment.
        TcpListener& operator=(const TcpListener&) = delete;

        /// @brief Start listening for connection at specified `address` and `port`.
        /// @param address Destination address.
        /// @param port Destination port.
        /// @param maxQueueLength Maximum connection requests.
        /// @return Whether start listening was successful.
        bool listen(const Address& address, uint16_t port, int maxQueueLength = 4096);

        /// @brief Accepts a new connection.
        /// @param[out] stream Buffer to store the connection stream.
        /// @return Whether accepting the connection and creating the stream was successful.
        /// @note This function will block until a connection is made.
        bool accept(TcpStream& stream) const;

        /// @brief Close the listener socket.
        void close();

        /// @brief Check if connection is valid.
        /// @return Whether inner sock is connected.
        inline bool valid() const
        {
            return mSock != InnerInvalidSocket;
        }

        /// @brief Get inner socket handle.
        /// @return Socket.
        inline InnerSocket inner() const
        {
            return mSock;
        }

        /// @brief Control blocking mode of inner socket.
        /// @note A blocking socket does not return control until it has sent (or received) some or all data specified
        /// for the operation.
        /// @param blocking Whether to block or not.
        /// @return Whether setting the block flag was sucessful.
        bool setBlocking(bool blocking);

    private:
        /// @brief Initializes the inner socket.
        /// @return Whether initialization was successful.
        bool create();

        InnerSocket mSock{InnerInvalidSocket};
        bool mBlocking{true};
    };
} // namespace cubos::core::net