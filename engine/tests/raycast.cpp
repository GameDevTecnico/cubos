#include <doctest/doctest.h>

#include <cubos/core/ecs/name.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/raycast.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/transform/position.hpp>

#include "utils.hpp"

using cubos::core::ecs::Name;
using cubos::core::ecs::World;
using namespace cubos::engine;

TEST_CASE("cubos::engine::Raycast")
{
    Cubos cubos{};
    cubos.plugin(settingsPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(fixedStepPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(collisionsPlugin);

    SUBCASE("get the right scalar from multiple boxes")
    {
        cubos.startupSystem("create resources").call([](Commands cmds) {
            cmds.create()
                .add(Name{"box1"})
                .add(Position{{0.0F, 0.0F, 0.0F}})
                .add(LocalToWorld{})
                .add(BoxCollisionShape{})
                .add(CollisionLayers{});

            cmds.create()
                .add(Name{"box2"})
                .add(Position{{1.0F, 0.0F, 0.0F}})
                .add(LocalToWorld{})
                .add(BoxCollisionShape{})
                .add(CollisionLayers{});

            cmds.create()
                .add(Name{"box3"})
                .add(Position{{0.0F, 0.0F, 1.0F}})
                .add(LocalToWorld{})
                .add(BoxCollisionShape{})
                .add(CollisionLayers{});

            cmds.create()
                .add(Name{"box4"})
                .add(Position{{-1.0F, 0.0F, 0.0F}})
                .add(LocalToWorld{})
                .add(BoxCollisionShape{})
                .add(CollisionLayers{});

            cmds.create()
                .add(Name{"box5"})
                .add(Position{{0.0F, 0.0F, -1.0F}})
                .add(LocalToWorld{})
                .add(BoxCollisionShape{})
                .add(CollisionLayers{});
        });

        cubos.system("raycast").after(transformUpdateTag).call([](const World& world, Raycast raycast) {
            auto hit2 = raycast.fire({{5.0F, 0.0F, 0.0F}, {-1.0F, 0.0F, 0.0F}});
            auto hit3 = raycast.fire({{0.0F, 0.0F, 5.0F}, {0.0F, 0.0F, -1.0F}});
            auto hit4 = raycast.fire({{-5.0F, 0.0F, 0.0F}, {1.0F, 0.0F, 0.0F}});
            auto hit5 = raycast.fire({{0.0F, 0.0F, -5.0F}, {0.0F, 0.0F, 1.0F}});

            CHECK(hit2.contains());
            CHECK(world.components(hit2->entity).get<Name>().value == "box2");
            CHECK_VEC3_EQ(hit2->point, glm::vec3{1.5F, 0.0F, 0.0F});

            CHECK(hit3.contains());
            CHECK(world.components(hit3->entity).get<Name>().value == "box3");
            CHECK_VEC3_EQ(hit3->point, glm::vec3{0.0F, 0.0F, 1.5F});

            CHECK(hit4.contains());
            CHECK(world.components(hit4->entity).get<Name>().value == "box4");
            CHECK_VEC3_EQ(hit4->point, glm::vec3{-1.5F, 0.0F, 0.0F});

            CHECK(hit5.contains());
            CHECK(world.components(hit5->entity).get<Name>().value == "box5");
            CHECK_VEC3_EQ(hit5->point, glm::vec3{0.0F, 0.0F, -1.5F});

            // box1 gets no hit
        });
    }

    SUBCASE("hit check with a single capsule")
    {
        cubos.startupSystem("create resources").call([](Commands cmds) {
            cmds.create()
                .add(Position{{0.0F, 0.0F, 0.0F}})
                .add(LocalToWorld{})
                .add(CapsuleCollisionShape{{1, 1}})
                .add(CollisionLayers{});
        });

        cubos.system("raycast").after(transformUpdateTag).call([](Raycast raycast) {
            auto verticalHit = raycast.fire({{0.0F, 5.0F, 0.0F}, {0.0F, -24.0F, 0.0F}});
            auto horizontalHit = raycast.fire({{-5.0F, 0.0F, 0.0F}, {3.0F, 0.0F, 0.0F}});
            auto diagCylinderHit = raycast.fire({{1.0F, 0.5F, 2.0F}, {-1.0F, -0.5F, -2.0F}});
            auto missCap = raycast.fire({{0.72F, 1.36F, 10.0F}, {0.0F, 0.0F, -3.2F}});
            auto diagCapHit = raycast.fire({{-0.99F, 3.0F, -0.84F}, {0.99F, -3.0F, 0.84F}});

            CHECK(verticalHit.contains());
            CHECK_VEC3_EQ(verticalHit->point, glm::vec3{0.0F, 1.5F, 0.0F});

            CHECK(horizontalHit.contains());
            CHECK_VEC3_EQ(horizontalHit->point, glm::vec3{-1.0F, 0.0F, 0.0F});

            CHECK_FALSE(missCap.contains());

            // this was first determined using Geogebra3D and verified by hand
            CHECK(diagCylinderHit.contains());
            CHECK(diagCylinderHit->point.x == doctest::Approx(0.447214F));
            CHECK(diagCylinderHit->point.y == doctest::Approx(0.223607F));
            CHECK(diagCylinderHit->point.z == doctest::Approx(0.894427F));

            CHECK(diagCapHit.contains());
            CHECK(diagCapHit->point.x == doctest::Approx(-0.435793F));
            CHECK(diagCapHit->point.y == doctest::Approx(1.32058F));
            CHECK(diagCapHit->point.z == doctest::Approx(-0.369764F));
        });
    }

    cubos.run();
}
