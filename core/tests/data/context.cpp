#include <doctest/doctest.h>

#include <cubos/core/data/context.hpp>

#include "utils.hpp"

using cubos::core::data::Context;

TEST_CASE("data::Context")
{
    SUBCASE("empty does not have int")
    {
        Context context{};
        CHECK(context.has<int>() == false);
    }

    SUBCASE("has int after pushing int")
    {
        Context context{};
        context.push<int>(1);
        CHECK(context.has<int>() == true);
        CHECK(context.get<int>() == 1);
        context.pop();
        CHECK(context.has<int>() == false);
    }

    SUBCASE("shadow int with another int")
    {
        Context context{};
        context.push<int>(1);
        context.push<int>(2);
        CHECK(context.get<int>() == 2);
        context.pop();
        CHECK(context.get<int>() == 1);
        context.pop();
        CHECK(context.has<int>() == false);
    }

    SUBCASE("push another context")
    {
        Context subContext{};
        subContext.push<int>(2);
        subContext.push<int>(3);

        Context context{};
        context.push(1);
        context.pushSubContext(subContext);
        CHECK(context.get<int>() == 3);
        context.pop(); // The entire subContext is popped, 2 is never read.
        CHECK(context.get<int>() == 1);
    }

    SUBCASE("destructor is called when popping")
    {
        bool first = false;
        bool second = false;
        Context context{};
        context.push(DetectDestructor{&first});
        context.push(DetectDestructor{&second});

        CHECK(first == false);
        CHECK(second == false);

        context.pop();
        CHECK(first == false);
        CHECK(second == true);

        context.pop();
        CHECK(first == true);
    }

    SUBCASE("destructor is called when context is destroyed")
    {
        bool destroyed = false;

        {
            Context context{};
            context.push(DetectDestructor{&destroyed});

            CHECK(destroyed == false);
        }

        CHECK(destroyed == true);
    }
}
