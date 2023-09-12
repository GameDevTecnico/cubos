#include <doctest/doctest.h>

#include <cubos/core/reflection/traits/constructible.hpp>

#include "../../utils.hpp"

using cubos::core::reflection::ConstructibleTrait;

struct SimpleConstructible
{
    static constexpr int DEFAULT = 0x12345678;

    int value;

    SimpleConstructible()
        : value(DEFAULT)
    {
    }
};

TEST_CASE("reflection::ConstructibleTrait")
{
    SUBCASE("size and alignment are correct")
    {
        auto trait = ConstructibleTrait::typed<DetectDestructor>().build();
        CHECK(trait.size() == sizeof(DetectDestructor));
        CHECK(trait.alignment() == alignof(DetectDestructor));
    }

    SUBCASE("destructor works")
    {
        auto ptr = operator new(sizeof(DetectDestructor));

        auto trait = ConstructibleTrait::typed<DetectDestructor>().build();
        bool destroyed = false;
        auto detector = new (ptr) DetectDestructor(&destroyed);

        REQUIRE_FALSE(destroyed);
        trait.destruct(detector);
        CHECK(destroyed);

        operator delete(ptr);
    }

    SUBCASE("non-assigned constructors work as expected")
    {
        auto ptr = operator new(sizeof(DetectDestructor));

        auto trait = ConstructibleTrait::typed<DetectDestructor>().build();
        DetectDestructor detector{};

        CHECK_FALSE(trait.defaultConstruct(ptr));
        CHECK_FALSE(trait.copyConstruct(ptr, &detector));
        CHECK_FALSE(trait.moveConstruct(ptr, &detector));

        operator delete(ptr);
    }

    SUBCASE("default constructor works")
    {
        auto ptr = operator new(sizeof(SimpleConstructible));

        auto trait = ConstructibleTrait::typed<SimpleConstructible>().withDefaultConstructor().build();
        REQUIRE(trait.defaultConstruct(ptr));

        auto simple = static_cast<SimpleConstructible*>(ptr);
        CHECK(simple->value == SimpleConstructible::DEFAULT);
        trait.destruct(ptr);

        operator delete(ptr);
    }

    SUBCASE("copy constructor works")
    {
        auto ptr = operator new(sizeof(SimpleConstructible));

        auto trait = ConstructibleTrait::typed<SimpleConstructible>().withCopyConstructor().build();
        SimpleConstructible copied{};
        copied.value = 1;
        REQUIRE(trait.copyConstruct(ptr, &copied));

        auto simple = static_cast<SimpleConstructible*>(ptr);
        CHECK(simple->value == copied.value);
        trait.destruct(ptr);

        operator delete(ptr);
    }

    SUBCASE("move constructor works")
    {
        auto ptr = operator new(sizeof(DetectDestructor));

        auto trait = ConstructibleTrait::typed<DetectDestructor>().withMoveConstructor().build();
        bool destroyed = false;

        // The destroyed flag is moved from the 'moved' detector to the detector at 'ptr', and thus
        // the flag is not set when 'moved' is destructed.
        {
            DetectDestructor moved{&destroyed};
            REQUIRE(trait.moveConstruct(ptr, &moved));
            REQUIRE_FALSE(destroyed);
        }

        CHECK_FALSE(destroyed);
        trait.destruct(ptr);
        CHECK(destroyed);

        operator delete(ptr);
    }
}
