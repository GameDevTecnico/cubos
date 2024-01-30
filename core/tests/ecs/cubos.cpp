#include <doctest/doctest.h>

#include <cubos/core/ecs/cubos.hpp>

#include "utils.hpp"

using namespace cubos::core::ecs;

TEST_CASE("ecs::Cubos")
{
    Cubos cubos;

    SUBCASE("duplicate plugin")
    {
        static int count = 0;
        auto plugin = [](Cubos&) { count += 1; };

        CHECK(count == 0);
        cubos.addPlugin(plugin);
        CHECK(count == 1);
        cubos.addPlugin(plugin);
        CHECK(count == 1);
    }

    SUBCASE("systems are called")
    {
        static int acc = 1;
        CHECK(acc == 1);
        cubos.startupSystem("mul 2").before("middle").call([]() { acc *= 2; });
        cubos.startupSystem("mul 3").tagged("middle").call([]() { acc *= 3; });
        cubos.startupSystem("mul 5").after("middle").call([]() { acc *= 5; });
        cubos.run();
        CHECK(acc == 30);
    }

    SUBCASE("single resource")
    {
        cubos.addResource<int>(0);
        cubos.startupSystem("check integer before").before("tag").call([](const int& x) { CHECK(x == 0); });
        cubos.startupSystem("increment integer").tagged("tag").call([](int& x) { x += 1; });
        cubos.startupSystem("check integer after").after("tag").call([](const int& x) { CHECK(x == 1); });
        cubos.run();
    }

    SUBCASE("calculate sum of ints")
    {
        cubos.addResource<int>(0);
        cubos.addComponent<int>();
        cubos.startupSystem("spawn stuff").tagged("spawn").call([](Commands cmds) {
            cmds.create().add<int>(1);
            cmds.create().add<int>(2);
            cmds.create().add<int>(3);
        });
        cubos.startupSystem("check sum before").after("spawn").before("tag").call([](const int& sum) {
            CHECK(sum == 0);
        });
        cubos.startupSystem("query stuff").tagged("tag").call([](int& sum, Query<const int&> query) {
            for (auto [x] : query)
            {
                sum += x;
            }
        });
        cubos.startupSystem("check sum after").after("tag").call([](const int& sum) { CHECK(sum == 6); });
        cubos.run();
    }
}
