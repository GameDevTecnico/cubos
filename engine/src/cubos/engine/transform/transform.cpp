#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/geom/decomposition.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/transform.hpp>

using namespace cubos::engine;

glm::mat4 Transform::localToWorld() const
{
    return mat;
}

glm::vec3 Transform::position() const
{
    return core::geom::position(mat);
}

void Transform::position(const glm::vec3& pos)
{
    core::geom::position(mat, pos);
}

void Transform::translate(const glm::vec3& delta)
{
    core::geom::translate(mat, delta);
}

glm::quat Transform::rotation() const
{
    return core::geom::rotation(mat);
}

glm::vec3 Transform::eulerRotation() const
{
    return core::geom::eulerRotation(mat);
}

void Transform::rotation(const glm::quat& rot)
{
    core::geom::rotation(mat, rot);
}

void Transform::rotation(const glm::vec3& euler)
{
    core::geom::rotation(mat, euler);
}

void Transform::rotate(const glm::quat& delta)
{
    core::geom::rotate(mat, delta);
}

void Transform::rotate(const glm::vec3& eulerDelta)
{
    core::geom::rotate(mat, eulerDelta);
}

float Transform::scale() const
{
    return core::geom::scale(mat);
}

void Transform::scale(float scale)
{
    core::geom::scale(mat, scale);
}

CUBOS_REFLECT_IMPL(cubos::engine::Transform)
{
    return core::ecs::ComponentTypeBuilder<Transform>("cubos::engine::Transform")
        .withField("mat", &Transform::mat)
        .build();
}
