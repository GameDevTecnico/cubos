/// @file
/// @brief Class @ref cubos::core::net::TcpStream.
/// @ingroup core-net

#pragma once

#include <cubos/core/api.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/net/address.hpp>
#include <cubos/core/net/utils.hpp>

namespace cubos::core::net
{
    /// @brief Stream implementation which writes to/reads from a TCP connection.
    /// @ingroup core-net
    class CUBOS_CORE_API TcpStream : public memory::Stream
    {
    public:
        /// @brief Constructs an empty TCP stream, ready for connecting.
        TcpStream();

        /// @brief Deconstructs.
        ~TcpStream() override;

        /// @brief Forbid copy construction.
        TcpStream(const TcpStream&) = delete;

        /// @brief Forbid copy assignment.
        TcpStream& operator=(const TcpStream&) = delete;

        /// @brief Connects to a TCP listener, possibly on a remote machine.
        /// @param address Destination address.
        /// @param port Destination port.
        /// @return Whether connecting was successful.
        bool connect(const Address& address, uint16_t port);

        /// @brief Replaces the inner socket with the given one.
        /// @param socket Socket.
        /// @return Whether connecting was successful.
        void inner(InnerSocket socket);

        /// @brief Disconnects inner socket.
        void disconnect();

        /// @brief Checks if inner socket is connected.
        /// @return Whether the buffer holds a valid connection.
        bool valid() const
        {
            return mSock != InnerInvalidSocket;
        }

        /// @brief Control blocking mode of inner socket.
        /// @note A blocking socket does not return control until it has sent (or received) some or all data specified
        /// for the operation.
        /// @param blocking Whether to block or not.
        /// @return Whether setting the block flag was sucessful.
        bool setBlocking(bool blocking);

        // Stream method implementations.

        std::size_t read(void* buffer, std::size_t size) override;

        std::size_t write(const void* buffer, std::size_t size) override;

        std::size_t tell() const override;

        bool eof() const override
        {
            return mEof;
        }

        char peek() override;

        void seek(ptrdiff_t offset, memory::SeekOrigin origin) override;

    private:
        InnerSocket mSock{InnerInvalidSocket};
        bool mBlocking{true};
        bool mEof{false};
    };
} // namespace cubos::core::net
