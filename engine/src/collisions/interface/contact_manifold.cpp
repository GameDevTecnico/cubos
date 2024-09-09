#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/collisions/contact_manifold.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::ContactPointData)
{
    return core::ecs::TypeBuilder<ContactPointData>("cubos::engine::ContactPointData")
        .withField("entity", &ContactPointData::entity)
        .withField("position1", &ContactPointData::position1)
        .withField("position2", &ContactPointData::position2)
        .withField("penetration", &ContactPointData::penetration)
        .withField("id", &ContactPointData::id)
        .build();
}

CUBOS_REFLECT_IMPL(cubos::engine::ContactManifold)
{
    return core::ecs::TypeBuilder<ContactManifold>("cubos::engine::ContactManifold")
        .symmetric()
        .withField("entity", &ContactManifold::entity)
        .withField("normal", &ContactManifold::normal)
        .withField("points", &ContactManifold::points)
        .build();
}
