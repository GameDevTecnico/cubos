#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <algorithm>
#include <cstring>

#include <cubos/core/net/address.hpp>
#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>
#include <cubos/core/reflection/type.hpp>

using namespace cubos::core::net;
using cubos::core::memory::Opt;

const Address Address::Any((uint8_t)0, 0, 0, 0);
const Address Address::LocalHost((uint8_t)127, 0, 0, 1);

CUBOS_REFLECT_IMPL(cubos::core::net::Address)
{
    using cubos::core::reflection::StringConversionTrait;
    using cubos::core::reflection::Type;

    return Type::create("cubos::core::net::Address")
        .with(StringConversionTrait{
            [](const void* instance) { return static_cast<const Address*>(instance)->toString(); },
            [](void* instance, const std::string& string) {
                auto addr = Address::from(string);
                if (addr.contains())
                {
                    *static_cast<Address*>(instance) = addr.value();
                    return true;
                }
                return false;
            }});
}

/// @brief Inits needed resources.
static void init()
{
#ifdef _WIN32
    WSADATA wsa;
    CUBOS_ASSERT(WSAStartup(MAKEWORD(2, 2), &wsa) == 0, " WSAStartup failed: {}", WSAGetLastError());
#endif
}

/// @brief Cleans up resources.
static void cleanup()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

Address::Address()
    : mType(IPType::IPv4)
{
}

Address::Address(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
    : mAddr{.v4Addr = static_cast<uint32_t>(a) << 24 | static_cast<uint32_t>(b) << 16 | static_cast<uint32_t>(c) << 8 |
                      static_cast<uint32_t>(d)}
    , mType(IPType::IPv4)
{
}

Address::Address(uint32_t address)
    : mAddr{.v4Addr = address}
    , mType(IPType::IPv4){};

Address::Address(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
    : mAddr{.v6Addr = {a, b, c, d}}
    , mType(IPType::IPv6)
{
}

Address::~Address()
{
    cleanup();
}

std::string Address::toString() const
{
    if (mType == IPType::IPv4)
    {
        char str[INET_ADDRSTRLEN];
        auto ip = htonl(mAddr.v4Addr);

        inet_ntop(AF_INET, &ip, str, INET_ADDRSTRLEN);
        return std::string{str};
    }

    char str[INET6_ADDRSTRLEN];
    std::array<uint32_t, 4> ip;

    std::transform(mAddr.v6Addr.begin(), mAddr.v6Addr.end(), ip.begin(), [](uint32_t v) { return htonl(v); });
    inet_ntop(AF_INET6, &ip, str, INET6_ADDRSTRLEN);
    return std::string{str};
}

Address Address::fromIPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    return {a, b, c, d};
}

Address Address::fromIPv6(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    return {a, b, c, d};
}

Opt<Address> Address::from(const std::string& address)
{
    init();

    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = PF_UNSPEC;
    hints.ai_flags |= AI_CANONNAME;

    struct addrinfo* res = nullptr;
    auto err = getaddrinfo(address.c_str(), nullptr, &hints, &res);
    if (err != 0)
    {
        CUBOS_ERROR("Failure to parse host '{}': {} ({})", address.c_str(), gai_strerror(err), err);
        return {};
    }

    if (res == nullptr)
    {
        CUBOS_ERROR("No host found for '{}'", address.c_str());
        return {};
    }

    struct sockaddr_storage result;
    memcpy(&result, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    cleanup();

    if (result.ss_family == AF_INET)
    {
        auto* sa4 = reinterpret_cast<struct sockaddr_in*>(&result);
        struct in_addr* in4 = &sa4->sin_addr;
        return Address(ntohl(in4->s_addr));
    }

    if (result.ss_family == AF_INET6)
    {
        auto* sa6 = reinterpret_cast<struct sockaddr_in6*>(&result);
        struct in6_addr* in6 = &sa6->sin6_addr;
#ifdef _WIN32
        // https://learn.microsoft.com/en-us/openspecs/windows_protocols/ms-dpdx/2b400962-05ef-4e69-9185-927755b322a5
        // The IN6_ADDR structure specifies an IPv6 transport address whose bytes are in network byte order
        // (big-endian).
        uint32_t a = in6->u.Word[1] << 16 | in6->u.Word[0];
        uint32_t b = in6->u.Word[3] << 16 | in6->u.Word[2];
        uint32_t c = in6->u.Word[5] << 16 | in6->u.Word[4];
        uint32_t d = in6->u.Word[7] << 16 | in6->u.Word[6];
        return Address((uint32_t)ntohl(a), ntohl(b), ntohl(c), ntohl(d));
#else
        uint32_t a = static_cast<uint32_t>(in6->s6_addr[3]) << 24 | static_cast<uint32_t>(in6->s6_addr[2]) << 16 |
                     static_cast<uint32_t>(in6->s6_addr[1]) << 8 | static_cast<uint32_t>(in6->s6_addr[0]);

        uint32_t b = static_cast<uint32_t>(in6->s6_addr[7]) << 24 | static_cast<uint32_t>(in6->s6_addr[6]) << 16 |
                     static_cast<uint32_t>(in6->s6_addr[5]) << 8 | static_cast<uint32_t>(in6->s6_addr[4]);

        uint32_t c = static_cast<uint32_t>(in6->s6_addr[11]) << 24 | static_cast<uint32_t>(in6->s6_addr[10]) << 16 |
                     static_cast<uint32_t>(in6->s6_addr[9]) << 8 | static_cast<uint32_t>(in6->s6_addr[8]);

        uint32_t d = static_cast<uint32_t>(in6->s6_addr[15]) << 24 | static_cast<uint32_t>(in6->s6_addr[14]) << 16 |
                     static_cast<uint32_t>(in6->s6_addr[13]) << 8 | static_cast<uint32_t>(in6->s6_addr[12]);

        return Address((uint32_t)ntohl(a), ntohl(b), ntohl(c), ntohl(d));
#endif
    }

    return {};
}