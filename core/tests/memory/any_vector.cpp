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

        SUBCASE("insertDefault/insertCopy/insertMove/erase")
        {
            int i = 2;
            vec.insertCopy(0, &i);
            i = 1;
            vec.insertMove(0, &i);
            vec.insertDefault(0);

            REQUIRE(vec.size() == 3);
            CHECK(*static_cast<const int*>(vec.at(0)) == 0);
            CHECK(*static_cast<const int*>(vec.at(1)) == 1);
            CHECK(*static_cast<const int*>(vec.at(2)) == 2);

            vec.insertDefault(2);
            REQUIRE(vec.size() == 4);
            CHECK(*static_cast<const int*>(vec.at(0)) == 0);
            CHECK(*static_cast<const int*>(vec.at(1)) == 1);
            CHECK(*static_cast<const int*>(vec.at(2)) == 0);
            CHECK(*static_cast<const int*>(vec.at(3)) == 2);

            vec.erase(2);
            REQUIRE(vec.size() == 3);
            CHECK(*static_cast<const int*>(vec.at(0)) == 0);
            CHECK(*static_cast<const int*>(vec.at(1)) == 1);
            CHECK(*static_cast<const int*>(vec.at(2)) == 2);

            vec.erase(2);
            REQUIRE(vec.size() == 2);
            CHECK(*static_cast<const int*>(vec.at(0)) == 0);
            CHECK(*static_cast<const int*>(vec.at(1)) == 1);

            vec.erase(0);
            REQUIRE(vec.size() == 1);
            CHECK(*static_cast<const int*>(vec.at(0)) == 1);

            vec.erase(0);
            REQUIRE(vec.size() == 0);
        }

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

        SUBCASE("copy constructor")
        {
            constexpr std::size_t Size = 100;

            for (std::size_t i = 0; i < Size; ++i)
            {
                vec.pushDefault();
                *static_cast<int*>(vec.at(i)) = static_cast<int>(i);
            }

            AnyVector vec2{vec};
            CHECK_FALSE(vec2.empty());
            CHECK(vec.size() == Size);
            CHECK(vec2.size() == Size);

            for (std::size_t i = 0; i < Size; ++i)
            {
                CHECK(*static_cast<const int*>(vec.at(i)) == static_cast<int>(i));
                CHECK(*static_cast<const int*>(vec2.at(i)) == static_cast<int>(i));
            }
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

        SUBCASE("swap erase until empty")
        {
            std::vector<std::size_t> indices{};
            for (std::size_t i = 0; i < Size; ++i)
            {
                indices.push_back(i);
            }

            while (!vec.empty())
            {
                auto index = indices[0];

                CHECK_FALSE(destroyed[index]);
                vec.swapErase(0);
                CHECK(destroyed[index]);

                indices[0] = indices.back();
                indices.pop_back();
            }
        }

        SUBCASE("swap move until empty")
        {
            std::vector<std::size_t> indices{};
            auto* detector = static_cast<DetectDestructor*>(operator new(sizeof(DetectDestructor)));

            for (std::size_t i = 0; i < Size; ++i)
            {
                indices.push_back(i);
            }

            while (!vec.empty())
            {

                auto index = indices[0];

                CHECK_FALSE(destroyed[index]);
                vec.swapMove(0, detector);
                CHECK_FALSE(destroyed[index]);
                detector->~DetectDestructor();
                CHECK(destroyed[index]);

                indices[0] = indices.back();
                indices.pop_back();
            }

            operator delete(detector);
        }

        SUBCASE("clear")
        {
            vec.clear();
        }

        SUBCASE("set to default")
        {
            for (std::size_t i = 0; i < Size; ++i)
            {
                CHECK_FALSE(destroyed[i]);
                vec.setDefault(i);
                CHECK(destroyed[i]);
            }

            vec.clear();
        }

        SUBCASE("set to copy")
        {
            bool flag = false;
            DetectDestructor detector{&flag};

            for (std::size_t i = 0; i < Size; ++i)
            {
                CHECK_FALSE(destroyed[i]);
                vec.setCopy(i, &detector);
                CHECK(destroyed[i]);
            }

            CHECK_FALSE(flag);
            vec.clear();
            CHECK(flag);
        }

        SUBCASE("set to move")
        {
            bool flag = false;

            for (std::size_t i = 0; i < Size; ++i)
            {
                DetectDestructor detector{&flag};

                CHECK_FALSE(destroyed[i]);
                vec.setMove(i, &detector);
                CHECK(destroyed[i]);
            }

            CHECK_FALSE(flag);
            vec.clear();
            CHECK(flag);
        }

        for (const auto& i : destroyed)
        {
            CHECK(i);
        }

        CHECK(vec.empty());
        CHECK(vec.size() == 0);
    }
}
