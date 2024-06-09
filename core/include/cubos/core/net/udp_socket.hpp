/// @file
/// @brief Class @ref cubos::core::net::UdpSocket.
/// @ingroup core-net

#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include <cubos/core/api.hpp>
#include <cubos/core/net/address.hpp>
#include <cubos/core/net/utils.hpp>

namespace cubos::core::net
{
    /// @brief Represents a UDP socket.
    /// @ingroup core-net
    class CUBOS_CORE_API UdpSocket
    {
    public:
        /// @brief Constructs a UDP socket.
        UdpSocket();

        /// @brief Deconstructs by unbinding the inner socket.
        ~UdpSocket();

        /// @brief Forbid copy construction.
        UdpSocket(const UdpSocket&) = delete;

        /// @brief Forbid copy assignment.
        UdpSocket& operator=(const UdpSocket&) = delete;

        /// @brief Binds a UDP socket from the given `port` and `address`.
        /// @param port Port to bind.
        /// @param address Address to bind.
        /// @return Whether binding was successful.
        bool bind(uint16_t port, const Address& address = Address::Any) const;

        /// @brief Closes the inner socket.
        void unbind();

        /// @brief Sends `data` on the socket to the remote `address` and `port`.
        /// @param data Message.
        /// @param size Message size.
        /// @param address Address to send.
        /// @param port Port to send.
        /// @return Whether data was sent sucessfully.
        bool send(const void* data, std::size_t size, const Address& address, uint16_t port) const;

        /// @brief Receives a single datagram message on the socket.
        /// @param data Buffer to store the datagram message.
        /// @param size Maximum buffer size.
        /// @param[out] received Number of bytes read.
        /// @param[out] address Origin address.
        /// @return Whether data was received sucessfully.
        bool receive(void* data, std::size_t size, std::size_t& received, Address& address) const;

        /// @brief Control blocking mode of inner socket.
        /// @note A blocking socket does not return control until it has sent (or received) some or all data specified
        /// for the operation.
        /// @note A non-blocking socket returns whatever is in the receive buffer and immediately continues.
        /// @param blocking Whether to block or not.
        /// @return Whether setting the block flag was sucessfull.
        bool setBlocking(bool blocking);

    private:
        InnerSocket mSock{InnerInvalidSocket};
        bool mBlocking{true};
    };
} // namespace cubos::core::net