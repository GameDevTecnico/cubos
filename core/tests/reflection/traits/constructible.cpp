#include <doctest/doctest.h>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

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

    SimpleConstructible(int x)
        : value(x)
    {
    }

    SimpleConstructible(int x, const int& y)
        : value(x + y)
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
        auto* ptr = operator new(sizeof(DetectDestructor));

        auto trait = ConstructibleTrait::typed<DetectDestructor>().build();
        bool destroyed = false;
        auto* detector = new (ptr) DetectDestructor(&destroyed);

        REQUIRE_FALSE(destroyed);
        trait.destruct(detector);
        CHECK(destroyed);

        operator delete(ptr);
    }

    SUBCASE("non-assigned constructors work as expected")
    {
        auto* ptr = operator new(sizeof(DetectDestructor));

        auto trait = ConstructibleTrait::typed<DetectDestructor>().build();
        DetectDestructor detector{};

        CHECK_FALSE(trait.hasDefaultConstruct());
        CHECK_FALSE(trait.hasCopyConstruct());
        CHECK_FALSE(trait.hasMoveConstruct());

        operator delete(ptr);
    }

    SUBCASE("default constructor works")
    {
        auto* ptr = operator new(sizeof(SimpleConstructible));

        auto trait = ConstructibleTrait::typed<SimpleConstructible>().withDefaultConstructor().build();
        REQUIRE(trait.hasDefaultConstruct());
        trait.defaultConstruct(ptr);

        auto* simple = static_cast<SimpleConstructible*>(ptr);
        CHECK(simple->value == SimpleConstructible::DEFAULT);
        trait.destruct(ptr);

        operator delete(ptr);
    }

    SUBCASE("copy constructor works")
    {
        auto* ptr = operator new(sizeof(SimpleConstructible));

        auto trait = ConstructibleTrait::typed<SimpleConstructible>().withCopyConstructor().build();
        SimpleConstructible copied{};
        copied.value = 1;
        REQUIRE(trait.hasCopyConstruct());
        trait.copyConstruct(ptr, &copied);

        auto* simple = static_cast<SimpleConstructible*>(ptr);
        CHECK(simple->value == copied.value);
        trait.destruct(ptr);

        operator delete(ptr);
    }

    SUBCASE("move constructor works")
    {
        auto* ptr = operator new(sizeof(DetectDestructor));

        auto trait = ConstructibleTrait::typed<DetectDestructor>().withMoveConstructor().build();
        bool destroyed = false;

        // The destroyed flag is moved from the 'moved' detector to the detector at 'ptr', and thus
        // the flag is not set when 'moved' is destructed.
        {
            DetectDestructor moved{&destroyed};
            REQUIRE(trait.hasMoveConstruct());
            trait.moveConstruct(ptr, &moved);
            REQUIRE_FALSE(destroyed);
        }

        CHECK_FALSE(destroyed);
        trait.destruct(ptr);
        CHECK(destroyed);

        operator delete(ptr);
    }

    SUBCASE("custom constructor works")
    {
        auto* ptr = operator new(sizeof(SimpleConstructible));

        auto trait = ConstructibleTrait::typed<SimpleConstructible>()
                         .withCustomConstructor<int>({"x"})
                         .withCustomConstructor<int, int>({"x", "y"})
                         .build();
        REQUIRE(trait.customConstructorCount() == 2);
        REQUIRE(trait.customConstructor(0).argCount() == 1);
        REQUIRE(trait.customConstructor(0).argName(0) == "x");
        REQUIRE(trait.customConstructor(0).argType(0).is<int>());
        REQUIRE(trait.customConstructor(1).argCount() == 2);
        REQUIRE(trait.customConstructor(1).argName(0) == "x");
        REQUIRE(trait.customConstructor(1).argName(1) == "y");
        REQUIRE(trait.customConstructor(1).argType(0).is<int>());
        REQUIRE(trait.customConstructor(1).argType(1).is<int>());

        // Test the first custom constructor.
        int x = 3;
        void* args[] = {&x};
        trait.customConstruct(0, ptr, args);
        auto* simple = static_cast<SimpleConstructible*>(ptr);
        CHECK(simple->value == x);
        trait.destruct(ptr);

        // Test the second custom constructor.
        int y = 4;
        int z = 5;
        void* args2[] = {&y, &z};
        trait.customConstruct(1, ptr, args2);
        auto* simple2 = static_cast<SimpleConstructible*>(ptr);
        CHECK(simple2->value == y + z);
        trait.destruct(ptr);

        operator delete(ptr);
    }
}
