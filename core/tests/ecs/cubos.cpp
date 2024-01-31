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

    SUBCASE("adding component filters through system options")
    {
        cubos.addComponent<int>();
        cubos.addComponent<bool>();

        cubos.startupSystem("spawn stuff").call([](Commands cmds) {
            cmds.create().add<int>(1).add(false);
            cmds.create().add<int>(2);
        });

        cubos.system("with bool").with<bool>().call([](Query<const int&> query) {
            CHECK(query.first().contains());
            auto [x] = *query.first();
            CHECK(x == 1);
        });

        cubos.system("without bool").without<bool>().call([](Query<const int&> query) {
            CHECK(query.first().contains());
            auto [x] = *query.first();
            CHECK(x == 2);
        });

        cubos.system("two queries, one with bool and another without")
            .with<bool>()
            .other()
            .without<bool>()
            .call([](Query<const int&> with, Query<const int&> without) {
                CHECK(with.first().contains());
                auto [x] = *with.first();
                CHECK(x == 1);

                CHECK(without.first().contains());
                auto [y] = *without.first();
                CHECK(y == 2);
            });

        cubos.run();
    }

    SUBCASE("changing traversal direction for tree relations")
    {
        cubos.addRelation<TreeRelation>();

        cubos.startupSystem("spawn stuff").call([](Commands cmds) {
            auto e1 = cmds.create().entity();
            auto e2 = cmds.create().entity();
            auto e3 = cmds.create().entity();
            cmds.relate(e1, e2, TreeRelation{});
            cmds.relate(e2, e3, TreeRelation{});
        });

        cubos.system("two queries, one up and another down")
            .entity()
            .related<TreeRelation>(Traversal::Up)
            .entity()
            .other()
            .entity()
            .related<TreeRelation>(Traversal::Down)
            .entity()
            .call([](Query<Entity, Entity> up, Query<Entity, Entity> down) {
                auto upIt = up.begin();
                CHECK(upIt != up.end());
                auto downIt = down.begin();
                CHECK(downIt != down.end());

                // upFrom1 = e1
                // upTo1 = e2
                auto [upFrom1, upTo1] = *upIt;
                CHECK(upFrom1 != upTo1);

                // downFrom1 = e2
                // downTo1 = e3
                auto [downFrom1, downTo1] = *downIt;
                CHECK(downFrom1 != downTo1);

                // upTo1 = downFrom1 = e2
                CHECK(upTo1 == downFrom1);
                CHECK(upFrom1 != downTo1);

                ++upIt;
                CHECK(upIt != up.end());
                ++downIt;
                CHECK(downIt != down.end());

                // upFrom2 = e2
                // upTo2 = e3
                auto [upFrom2, upTo2] = *upIt;
                CHECK(upFrom2 != upTo2);
                CHECK(upFrom2 == downFrom1);
                CHECK(upTo2 == downTo1);

                // downFrom2 = e1
                // downTo2 = e2
                auto [downFrom2, downTo2] = *downIt;
                CHECK(downFrom2 != downTo2);
                CHECK(downFrom2 == upFrom1);
                CHECK(downTo2 == upTo1);

                ++upIt;
                CHECK(upIt == up.end());
                ++downIt;
                CHECK(downIt == down.end());
            });

        cubos.run();
    }
}
