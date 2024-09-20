#include <doctest/doctest.h>

#include <cubos/core/memory/function.hpp>
#include <cubos/core/memory/move.hpp>

#include "../utils.hpp"

namespace
{
    struct Moveable
    {
        Moveable() = default;
        Moveable(const Moveable&) = delete;

        Moveable(Moveable&& /*unused*/) noexcept
        {
            moved = true;
        }

        bool moved{false};
    };
} // namespace

TEST_CASE("memory::Function")
{
    using cubos::core::memory::Function;
    using cubos::core::memory::move;

    SUBCASE("simple argument")
    {
        Function<bool(int)> func{};

        int acc = 0;
        CHECK(func == nullptr);
        func = [&acc](int) { return ++acc < 5; };
        CHECK(func != nullptr);
        while (func(0))
        {
        }
        CHECK(acc == 5);
    }

    SUBCASE("moved argument")
    {
        Function<Moveable(Moveable)> func{};
        CHECK(func == nullptr);
        func = [](Moveable moveable) { return moveable; };
        CHECK(func != nullptr);

        // Handles moving stuff around
        CHECK(func(Moveable{}).moved);
    }

    SUBCASE("reference argument")
    {
        Function<void(int&)> setter{};

        CHECK(setter == nullptr);
        setter = [](int& ref) { ref = 1; };
        CHECK(setter != nullptr);

        int val = 0;
        setter(val);
        CHECK(val == 1);

        setter = [](int& ref) { ref = 2; };
        CHECK(setter != nullptr);
        setter(val);
        CHECK(val == 2);
    }

    SUBCASE("destructor works")
    {
        bool flag = false;
        DetectDestructor detector{&flag};

        CHECK_FALSE(flag);
        Function<void()> function = [detector = move(detector)]() {};
        CHECK_FALSE(flag);
        function();
        CHECK_FALSE(flag);
        function = nullptr;
        CHECK(flag);
    }

    SUBCASE("move constructor works for mutable functions")
    {
        Function<void()> function = nullptr;
        CHECK(function == nullptr);

        function = []() {};
        CHECK(function != nullptr);

        Function<void()> movedFunction = move(function);
        CHECK(function == nullptr);
        CHECK(movedFunction != nullptr);

        function = move(movedFunction);
        CHECK(function != nullptr);
        CHECK(movedFunction == nullptr);
    }

    SUBCASE("move constructor works for const functions")
    {
        Function<void() const> function = nullptr;
        CHECK(function == nullptr);

        function = []() {};
        CHECK(function != nullptr);

        Function<void() const> movedFunction = move(function);
        CHECK(function == nullptr);
        CHECK(movedFunction != nullptr);

        function = move(movedFunction);
        CHECK(function != nullptr);
        CHECK(movedFunction == nullptr);
    }
}
