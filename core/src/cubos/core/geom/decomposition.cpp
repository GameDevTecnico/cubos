#include <glm/gtx/euler_angles.hpp>

#include <cubos/core/geom/decomposition.hpp>

static glm::vec3 cubos::core::geom::position(const glm::mat4& mat)
{
    return static_cast<glm::vec3>(mat[3]);
}

static void cubos::core::geom::position(glm::mat4& mat, const glm::vec3& pos)
{
    mat[3] = glm::vec4(pos, mat[3].w);
}

static void cubos::core::geom::translate(glm::mat4& mat, const glm::vec3& delta)
{
    mat[3] += glm::vec4(delta, 0.0F);
}

static glm::quat cubos::core::geom::rotation(const glm::mat4& mat)
{
    return glm::quat_cast(static_cast<glm::mat3>(mat) / scale(mat));
}

static glm::vec3 cubos::core::geom::eulerRotation(const glm::mat4& mat)
{
    return glm::eulerAngles(rotation(mat));
}

static void cubos::core::geom::rotation(glm::mat4& mat, const glm::quat& rot)
{
    auto p = position(mat);
    mat = glm::mat4_cast(rot) * scale(mat);
    translate(mat, p);
}

static void cubos::core::geom::rotation(glm::mat4& mat, const glm::vec3& euler)
{
    rotation(mat, glm::quat(euler));
}

static void cubos::core::geom::rotate(glm::mat4& mat, const glm::quat& delta)
{
    mat *= glm::mat4_cast(delta);
}

static void cubos::core::geom::rotate(glm::mat4& mat, const glm::vec3& eulerDelta)
{
    rotate(mat, glm::yawPitchRoll(eulerDelta.x, eulerDelta.y, eulerDelta.z));
}

static float cubos::core::geom::scale(const glm::mat4& mat)
{
    return glm::determinant(static_cast<glm::mat3x3>(mat));
}

static void cubos::core::geom::scale(glm::mat4& mat, float scale)
{
    auto rs = static_cast<glm::mat4>(glm::mat3_cast(rotation(mat)) * glm::mat3(scale));
    mat[0] = rs[0];
    mat[1] = rs[1];
    mat[2] = rs[2];
}