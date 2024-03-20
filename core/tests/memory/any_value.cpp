#include <doctest/doctest.h>

#include <cubos/core/memory/any_value.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/type.hpp>

#include "../utils.hpp"

TEST_CASE("memory::AnyValue")
{
    using cubos::core::memory::AnyValue;
    using cubos::core::reflection::reflect;

    SUBCASE("with integers")
    {
        SUBCASE("customConstruct")
        {
            const auto any = AnyValue::customConstruct<int>(1337);
            CHECK(any.type().is<int>());
            CHECK(*static_cast<const int*>(any.get()) == 1337);
        }

        SUBCASE("defaultConstruct")
        {
            const auto any = AnyValue::defaultConstruct(reflect<int>());
            CHECK(any.type().is<int>());
            CHECK(*static_cast<const int*>(any.get()) == 0);
        }

        SUBCASE("copyConstruct")
        {
            int value = 1337;
            auto any = AnyValue::copyConstruct(reflect<int>(), &value);
            CHECK(any.type().is<int>());
            CHECK(*static_cast<int*>(any.get()) == value);
        }

        SUBCASE("moveConstruct")
        {
            int value = 1337;
            auto any = AnyValue::moveConstruct(reflect<int>(), &value);
            CHECK(any.type().is<int>());
            CHECK(*static_cast<int*>(any.get()) == value);
        }

        SUBCASE("move assignment")
        {
            int value = 1337;
            AnyValue any{};
            REQUIRE_FALSE(any.valid());
            any = AnyValue::moveConstruct(reflect<int>(), &value);
            REQUIRE(any.valid());
            CHECK(any.type().is<int>());
            CHECK(*static_cast<int*>(any.get()) == value);
        }
    }

    SUBCASE("detect destructor")
    {
        bool destructed = false;
        DetectDestructor detector{&destructed};

        {
            auto any = AnyValue::moveConstruct(reflect<DetectDestructor>(), &detector);
            CHECK_FALSE(destructed);
        }

        CHECK(destructed);
    }
}
