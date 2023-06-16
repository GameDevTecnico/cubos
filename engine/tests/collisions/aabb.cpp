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
    commands.create(BoxCollider{glm::mat4{1.0f}, Box{glm::vec3{1.0f}}});
    commands.create(CapsuleCollider{glm::mat4{1.0f}, Capsule{1.0f, 1.0f}});
    commands.create(SimplexCollider{glm::vec3{0.0f}, Simplex{{glm::vec3{1.0f}, glm::vec3{2.0f}, glm::vec3{3.0f}}}});
    commands.create(PlaneCollider{glm::vec3{0.0f}, Plane{glm::vec3{1.0f}}});
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
    cubos.system(setup).beforeTag("cubos.collisions.aabb.missing");

    SUBCASE("collisions.aabb.missing: missing aabbs were added")
    {
        cubos.system(testAddMissingAABBs<BoxCollider>).afterTag("cubos.collisions.aabb.missing");
        cubos.system(testAddMissingAABBs<CapsuleCollider>).afterTag("cubos.collisions.aabb.missing");
        cubos.system(testAddMissingAABBs<SimplexCollider>).afterTag("cubos.collisions.aabb.missing");
        cubos.system(testAddMissingAABBs<PlaneCollider>).afterTag("cubos.collisions.aabb.missing");
    }

    cubos.run();
}