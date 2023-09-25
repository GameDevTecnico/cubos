#include <doctest/doctest.h>
#include <glm/glm.hpp>

#include <cubos/engine/collisions/aabb.hpp>
#include <cubos/engine/collisions/colliders/box.hpp>
#include <cubos/engine/collisions/colliders/capsule.hpp>
#include <cubos/engine/collisions/colliders/plane.hpp>
#include <cubos/engine/collisions/colliders/simplex.hpp>
#include <cubos/engine/collisions/plugin.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::OptRead;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::geom::Box;
using cubos::core::geom::Capsule;
using cubos::core::geom::Plane;
using cubos::core::geom::Simplex;
using namespace cubos::engine;

static void setup(Commands commands)
{
    commands.create(BoxCollider{glm::mat4{1.0F}, Box{glm::vec3{1.0F}}});
    commands.create(CapsuleCollider{glm::mat4{1.0F}, Capsule{1.0F, 1.0F}});
    commands.create(SimplexCollider{glm::vec3{0.0F}, Simplex{{glm::vec3{1.0F}, glm::vec3{2.0F}, glm::vec3{3.0F}}}});
    commands.create(PlaneCollider{glm::vec3{0.0F}, Plane{glm::vec3{1.0F}}});
}

template <typename C>
void testAddMissingAABBs(Query<Read<C>, OptRead<ColliderAABB>> query)
{
    for (auto [entity, collider, aabb] : query)
    {
        CHECK(static_cast<bool>(aabb));
    }
}

TEST_CASE("collisions.aabb")
{
    auto cubos = Cubos{};

    cubos.addPlugin(collisionsPlugin);
    cubos.system(setup).before("cubos.collisions.aabb.missing");

    SUBCASE("collisions.aabb.missing: missing aabbs were added")
    {
        cubos.system(testAddMissingAABBs<BoxCollider>).after("cubos.collisions.aabb.missing");
        cubos.system(testAddMissingAABBs<CapsuleCollider>).after("cubos.collisions.aabb.missing");
        cubos.system(testAddMissingAABBs<SimplexCollider>).after("cubos.collisions.aabb.missing");
        cubos.system(testAddMissingAABBs<PlaneCollider>).after("cubos.collisions.aabb.missing");
    }

    cubos.run();
}