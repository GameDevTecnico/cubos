#include <glm/gtx/quaternion.hpp>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/local_to_world.hpp>

CUBOS_REFLECT_IMPL(cubos::engine::LocalToWorld)
{
    return core::ecs::TypeBuilder<LocalToWorld>("cubos::engine::LocalToWorld")
        .ephemeral()
        .withField("mat", &LocalToWorld::mat)
        .build();
}

glm::mat4 cubos::engine::LocalToWorld::inverse() const
{
    return glm::inverse(mat);
}

glm::vec3 cubos::engine::LocalToWorld::worldPosition() const
{
    return mat[3];
}

glm::quat cubos::engine::LocalToWorld::worldRotation() const
{
    return glm::toQuat(glm::mat3(mat) / worldScale());
}

float cubos::engine::LocalToWorld::worldScale() const
{
    return glm::length(mat[0]);
}