#include <doctest/doctest.h>

#include <cubos/core/memory/any_vector.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/type.hpp>

#include "../utils.hpp"

TEST_CASE("memory::AnyVector")
{
    using cubos::core::memory::AnyVector;
    using cubos::core::reflection::reflect;

    SUBCASE("with integers")
    {
        AnyVector vec{reflect<int>()};
        CHECK(vec.elementType().is<int>());

        SUBCASE("pushDefault")
        {
            vec.pushDefault();
            CHECK(vec.size() == 1);
            CHECK(vec.at(0) != nullptr);
            CHECK(*static_cast<const int*>(vec.at(0)) == 0);
        }

        SUBCASE("pushCopy/pushMove")
        {
            int value = 42;

            SUBCASE("pushCopy")
            {
                vec.pushCopy(&value);
            }

            SUBCASE("pushMove")
            {
                vec.pushMove(&value);
            }

            CHECK(vec.size() == 1);
            CHECK(vec.at(0) != nullptr);
            CHECK(*static_cast<const int*>(vec.at(0)) == 42);
        }

        SUBCASE("move constructor")
        {
            constexpr std::size_t Size = 100;

            for (std::size_t i = 0; i < Size; ++i)
            {
                vec.pushDefault();
                *static_cast<int*>(vec.at(i)) = static_cast<int>(i);
            }

            AnyVector vec2{std::move(vec)};
            CHECK_FALSE(vec2.empty());
            CHECK(vec2.size() == Size);

            for (std::size_t i = 0; i < Size; ++i)
            {
                CHECK(*static_cast<const int*>(vec2.at(i)) == static_cast<int>(i));
            }
        }
    }

    SUBCASE("detect destructor")
    {
        AnyVector vec{reflect<DetectDestructor>()};
        CHECK(vec.elementType().is<DetectDestructor>());

        constexpr std::size_t Size = 100;
        bool destroyed[Size];

        for (std::size_t i = 0; i < Size; ++i)
        {
            CHECK(vec.size() == i);
            vec.pushDefault();
            CHECK(vec.size() == i + 1);
            CHECK(vec.capacity() >= i + 1);

            destroyed[i] = false;
            static_cast<DetectDestructor*>(vec.at(i))->set(&destroyed[i]);
        }

        SUBCASE("pop until empty")
        {
            for (std::size_t i = 0; i < Size; ++i)
            {
                CHECK_FALSE(destroyed[Size - i - 1]);
                vec.pop();
                CHECK(destroyed[Size - i - 1]);
            }
        }

        SUBCASE("clear")
        {
            vec.clear();
        }

        for (const auto& i : destroyed)
        {
            CHECK(i);
        }

        CHECK(vec.empty());
        CHECK(vec.size() == 0);
    }
}
