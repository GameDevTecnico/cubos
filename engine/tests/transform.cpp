#include <doctest/doctest.h>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

TEST_CASE("cubos::engine::transformPlugin")
{
    Cubos cubos{};
    cubos.addPlugin(transformPlugin);

    SUBCASE("main components are added automatically")
    {
        SUBCASE("from Position")
        {
            cubos.startupSystem("create entity w/Position").call([](Commands cmds) { cmds.create().add(Position{}); });
        }

        SUBCASE("from Rotation")
        {
            cubos.startupSystem("create entity w/Rotation").call([](Commands cmds) { cmds.create().add(Rotation{}); });
        }

        SUBCASE("from Scale")
        {
            cubos.startupSystem("create entity w/Scale").call([](Commands cmds) { cmds.create().add(Scale{}); });
        }

        SUBCASE("from LocalToWorld")
        {
            cubos.startupSystem("create entity w/LocalToWorld").call([](Commands cmds) {
                cmds.create().add(LocalToWorld{});
            });
        }

        cubos.system("check if there's a single entity with all of the components")
            .after(transformUpdateTag)
            .with<Position>()
            .with<Rotation>()
            .with<Scale>()
            .with<LocalToWorld>()
            .call([](Query<> query) { CHECK(query.first().contains()); });

        cubos.run();
    }

    SUBCASE("LocalToParent is added automatically")
    {
        cubos.startupSystem("create entity w/LocalToWorld, child of another entity w/LocalToWorld")
            .call([](Commands cmds) {
                auto child = cmds.create().add(LocalToWorld{}).entity();
                auto parent = cmds.create().add(LocalToWorld{}).entity();
                cmds.relate(child, parent, ChildOf{});
            });

        cubos.system("check if there's a single entity pair with all of the components")
            .after(transformUpdateTag)
            .with<Position>()
            .with<Rotation>()
            .with<Scale>()
            .with<LocalToWorld>()
            .with<LocalToParent>()
            .related<ChildOf>()
            .with<Position>()
            .with<Rotation>()
            .with<Scale>()
            .with<LocalToWorld>()
            .call([](Query<> query) { CHECK(query.first().contains()); });

        cubos.run();
    }

    SUBCASE("LocalToWorld is calculated correctly for root entities")
    {
        cubos.startupSystem("create entity with Position, Rotation and Scale").call([](Commands cmds) {
            cmds.create()
                .add(Position{{1.0F, 0.0F, 0.0F}})
                .add(Rotation{glm::angleAxis(glm::pi<float>() / 2, glm::vec3{0.0F, 1.0F, 0.0F})})
                .add(Scale{2.0F});
        });

        cubos.system("check if there's a single entity pair with all of the components")
            .after(transformUpdateTag)
            .call([](Query<const LocalToWorld&> query) {
                REQUIRE(query.first().contains());
                auto [localToWorld] = *query.first();

                auto point = localToWorld.mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
                CHECK(point.x == doctest::Approx(1.0F));
                CHECK(point.y == doctest::Approx(0.0F));
                CHECK(point.z == doctest::Approx(0.0F));
                CHECK(point.w == doctest::Approx(1.0F));

                point = localToWorld.mat * glm::vec4(1.0F, 0.0F, 0.0F, 1.0F);
                CHECK(point.x == doctest::Approx(1.0F));
                CHECK(point.y == doctest::Approx(0.0F));
                CHECK(point.z == doctest::Approx(-2.0F));
                CHECK(point.w == doctest::Approx(1.0F));
            });

        cubos.run();
    }

    SUBCASE("LocalToWorld/LocalToParent is calculated correctly for child entities")
    {
        cubos.startupSystem("create parent and child entities with Position, Rotation and Scale")
            .call([](Commands cmds) {
                auto child = cmds.create().add(Position{{1.0F, 0.0F, 0.0F}}).entity();
                auto parent = cmds.create().add(Position{{3.0F, 0.0F, 0.0F}}).entity();
                auto grandparent = cmds.create().add(Position{{5.0F, 0.0F, 0.0F}}).entity();

                cmds.relate(child, parent, ChildOf{});
                cmds.relate(parent, grandparent, ChildOf{});
            });

        cubos.system("check if both entity pairs are correct")
            .after(transformUpdateTag)
            .call([](Query<const Position&, Opt<const LocalToParent&>, const LocalToWorld&> query) {
                for (auto [position, localToParent, localToWorld] : query)
                {
                    if (position.vec.x == doctest::Approx(1.0F))
                    {
                        auto point = localToWorld.mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
                        CHECK(point.x == doctest::Approx(9.0F));
                        CHECK(point.y == doctest::Approx(0.0F));
                        CHECK(point.z == doctest::Approx(0.0F));
                        CHECK(point.w == doctest::Approx(1.0F));

                        REQUIRE(localToParent);
                        point = localToParent->mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
                        CHECK(point.x == doctest::Approx(1.0F));
                        CHECK(point.y == doctest::Approx(0.0F));
                        CHECK(point.z == doctest::Approx(0.0F));
                        CHECK(point.w == doctest::Approx(1.0F));
                    }
                    else if (position.vec.x == doctest::Approx(3.0F))
                    {
                        auto point = localToWorld.mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
                        CHECK(point.x == doctest::Approx(8.0F));
                        CHECK(point.y == doctest::Approx(0.0F));
                        CHECK(point.z == doctest::Approx(0.0F));
                        CHECK(point.w == doctest::Approx(1.0F));

                        REQUIRE(localToParent);
                        point = localToParent->mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
                        CHECK(point.x == doctest::Approx(3.0F));
                        CHECK(point.y == doctest::Approx(0.0F));
                        CHECK(point.z == doctest::Approx(0.0F));
                        CHECK(point.w == doctest::Approx(1.0F));
                    }
                    else if (position.vec.x == doctest::Approx(5.0F))
                    {
                        auto point = localToWorld.mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
                        CHECK(point.x == doctest::Approx(5.0F));
                        CHECK(point.y == doctest::Approx(0.0F));
                        CHECK(point.z == doctest::Approx(0.0F));
                        CHECK(point.w == doctest::Approx(1.0F));

                        REQUIRE(localToParent);
                        point = localToParent->mat * glm::vec4(0.0F, 0.0F, 0.0F, 1.0F);
                        CHECK(point.x == doctest::Approx(5.0F));
                        CHECK(point.y == doctest::Approx(0.0F));
                        CHECK(point.z == doctest::Approx(0.0F));
                        CHECK(point.w == doctest::Approx(1.0F));
                    }
                }
            });

        cubos.run();
    }
}
