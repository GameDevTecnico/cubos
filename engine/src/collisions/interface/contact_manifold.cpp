#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/collisions/contact_manifold.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::ContactFeatureId)
{
    return core::ecs::TypeBuilder<ContactFeatureId>("cubos::engine::ContactFeatureId")
        .withField("id", &ContactFeatureId::id)
        .build();
}

CUBOS_REFLECT_IMPL(cubos::engine::ContactPointData)
{
    return core::ecs::TypeBuilder<ContactPointData>("cubos::engine::ContactPointData")
        .withField("entity", &ContactPointData::entity)
        .withField("globalOn1", &ContactPointData::globalOn1)
        .withField("globalOn2", &ContactPointData::globalOn2)
        .withField("localOn1", &ContactPointData::localOn1)
        .withField("localOn2", &ContactPointData::localOn2)
        .withField("penetration", &ContactPointData::penetration)
        .withField("normalImpulse", &ContactPointData::normalImpulse)
        .withField("frictionImpulse1", &ContactPointData::frictionImpulse1)
        .withField("frictionImpulse2", &ContactPointData::frictionImpulse2)
        .withField("fid1", &ContactPointData::fid1)
        .withField("fid2", &ContactPointData::fid2)
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
