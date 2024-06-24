/// @file
/// @brief Class @ref cubos::core::net::Address.
/// @ingroup core-net

#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <cubos/core/api.hpp>
#include <cubos/core/memory/opt.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::net
{
    /// @brief Represents an address, either IPv4 or IPv6.
    /// @ingroup core-net
    class CUBOS_CORE_API Address
    {
    public:
        CUBOS_REFLECT;

        /// @brief 0.0.0.0
        static const Address Any;

        /// @brief 127.0.0.1
        static const Address LocalHost;

        /// @brief Constructs a empty address (IPv4 by default).
        Address();

        /// @brief Deconstructs by cleaning necessary resources.
        ~Address();

        /// @brief Constructs a new IPv4 address from four octets (`a.b.c.d`).
        /// @param a First octet.
        /// @param b Second octet.
        /// @param c Third octet.
        /// @param d Fourth octet.
        static Address fromIPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

        /// @brief Constructs a new IPv6 address from four 32-bit integers (`a.b.c.d`).
        /// @param a First octet.
        /// @param b Second octet.
        /// @param c Third octet.
        /// @param d Fourth octet.
        static Address fromIPv6(uint32_t a, uint32_t b, uint32_t c, uint32_t d);

        /// @brief Converts the provided string value to an Address.
        /// @param address The address, either in IPv4/IPv6 format, or as a domain hostname.
        /// @return Address object, or nothing if could not resolve the provided `address`.
        static memory::Opt<Address> from(const std::string& address);

        /// @brief Converts the address value to a string.
        /// @return Address represented as a string.
        std::string toString() const;

    private:
        /// @copydoc cubos::core::net::fromIPv4
        Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d);

        /// @copydoc cubos::core::net::fromIPv6
        Address(uint32_t a, uint32_t b, uint32_t c, uint32_t d);

        /// @brief Constructs a new IPv4 address from the given value.
        /// @param address Address in 32-bit format (octets together basically).
        Address(uint32_t address);

        enum class IPType
        {
            IPv4,
            IPv6
        };

        union A {
            uint32_t v4Addr;
            std::array<uint32_t, 4> v6Addr;
        };

        A mAddr{};
        IPType mType{};
    };

} // namespace cubos::core::net
