#include <doctest/doctest.h>

#include <cubos/core/net/address.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/traits/string_conversion.hpp>

TEST_CASE("net::Address")
{
    using cubos::core::net::Address;
    using cubos::core::reflection::reflect;
    using cubos::core::reflection::StringConversionTrait;

    SUBCASE("from and to string ipv4")
    {
        auto addr = Address::from("127.0.0.1");
        REQUIRE(addr.contains());
        REQUIRE(addr.value().toString() == "127.0.0.1");

        REQUIRE(!Address::from("abc123").contains());
    }

    SUBCASE("from and to string ipv6")
    {
        auto addr = Address::from("::1");
        REQUIRE(addr.contains());
        REQUIRE(addr.value().toString() == "::1");

        REQUIRE(!Address::from("abc123").contains());
    }

    SUBCASE("from and to string hostname")
    {
        auto addr = Address::from("localhost");
        REQUIRE(addr.contains());
        auto expected = addr.value().toString() == "127.0.0.1" || addr.value().toString() == "::1";
        REQUIRE(expected);
    }

    SUBCASE("empty address")
    {
        Address addr;
        REQUIRE(addr.toString() == "0.0.0.0");
    }

    SUBCASE("octets separate")
    {
        auto addr = Address::fromIPv4(127, 0, 0, 1);
        REQUIRE(addr.toString() == "127.0.0.1");

        const auto& addr2 = addr;
        REQUIRE(addr2.toString() == "127.0.0.1");

        auto addr3 = Address::fromIPv6(0, 0, 0, 1);
        REQUIRE(addr3.toString() == "::1");
    }

    SUBCASE("localhost and any")
    {
        REQUIRE(Address::Any.toString() == Address::fromIPv4(0, 0, 0, 0).toString());
        REQUIRE(Address::LocalHost.toString() == Address::fromIPv4(127, 0, 0, 1).toString());
    }

    SUBCASE("from domain name")
    {
        auto addr = Address::from("www.google.com");
        REQUIRE(addr.contains());
    }

    SUBCASE("reflection string trait")
    {
        auto addr = Address::fromIPv4(127, 0, 0, 1);

        const auto& r = reflect<Address>();
        REQUIRE(r.name() == "cubos::core::net::Address");
        REQUIRE(r.has<StringConversionTrait>());

        const auto& strTrait = r.get<StringConversionTrait>();
        REQUIRE(strTrait.into(&addr) == "127.0.0.1");
        REQUIRE(strTrait.from(&addr, "0.0.0.0"));
        REQUIRE(addr.toString() == "0.0.0.0");

        REQUIRE(!strTrait.from(&addr, "aaa"));
    }
}
