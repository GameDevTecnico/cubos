#include <glm/glm.hpp>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/collisions/shapes/voxel.hpp>
#include <cubos/engine/physics/components/inertia.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(Inertia)
{
    return cubos::core::ecs::TypeBuilder<cubos::engine::Inertia>("cubos::engine::Inertia")
        .withField("inertia", &Inertia::inertia)
        .withField("inverseInertia", &Inertia::inverseInertia)
        .withField("autoUpdate", &Inertia::autoUpdate)
        .build();
}

void Inertia::setFromTensor(const glm::mat3 inertiaTensor)
{
    // TODO: check is is diagonal, if not apply the Jacobi eigenvalue algorithm.
    inertia = inertiaTensor;
    inverseInertia = glm::inverse(inertia);
}

void Inertia::setWithLocalFrame(const glm::vec3 principalInertia, const glm::quat principalLocalFrame)
{
    CUBOS_ASSERT(glm::any(glm::lessThan(principalInertia, {0.0F, 0.0F, 0.0F})),
                 "All values in principal inertia must be non negative");

    inertia =
        glm::mat3(principalLocalFrame) *
        glm::mat3(principalInertia.x, 0.0F, 0.0F, 0.0F, principalInertia.y, 0.0F, 0.0F, 0.0F, principalInertia.z) *
        glm::mat3(glm::inverse(principalLocalFrame));
    inverseInertia = glm::inverse(inertia);
}

glm::mat3 Inertia::rotatedInertia(const glm::quat& rotationQuat) const
{
    auto rotationMat = glm::mat3(rotationQuat);
    return rotationMat * inertia * glm::transpose(rotationMat);
}

glm::mat3 Inertia::rotatedInverseInertia(const glm::quat& rotationQuat) const
{
    auto rotationMat = glm::mat3(rotationQuat);
    return rotationMat * inverseInertia * glm::transpose(rotationMat);
}

/// @brief Calculates the volume of a box shape.
/// @param dimensions The width, height and lenght of the box.
/// @return The volume of the box with the given `dimensions`.
inline static float boxShapeVolume(const glm::vec3& dimensions)
{
    return dimensions.x * dimensions.y * dimensions.z;
}

/// @brief Calculates the volume of a ball shape.
/// @param radius of the shape.
/// @return The volume of the ball with the given `radius`.
inline static float ballShapeVolume(float radius)
{
    return glm::pi<float>() * radius * radius * radius * 4.0F / 3.0F;
}

/// @brief Calculates the volume of a cylinder shape.
/// @param height of the cylinder shape.
/// @param radius of the cylinder shape.
/// @return The volume of the cylinder with the given `height` and `radius`.
inline static float cylinderShapeVolume(float height, float radius)
{
    return glm::pi<float>() * radius * radius * height;
}

/// @brief Calculates the volume of a voxel shape.
/// @param shape The voxel collision shape.
/// @return The volume of the box with the given `dimensions`
static float voxelShapeVolume(const VoxelCollisionShape& shape)
{
    float volume = 0.0F;
    for (auto box : shape.getBoxes())
    {
        volume += boxShapeVolume(box.box.halfSize * 2.0F);
    }
    return volume;
}

glm::mat3 cubos::engine::boxShapeInertiaTensor(const float mass, const glm::vec3 dimensions)
{
    float constant = mass / 12.0F;
    auto x2 = (float)glm::pow(dimensions.x, 2);
    auto y2 = (float)glm::pow(dimensions.y, 2);
    auto z2 = (float)glm::pow(dimensions.z, 2);

    return glm::mat3{constant * (z2 + y2), 0.0F, 0.0F, 0.0F, constant * (x2 + z2), 0.0F, 0.0F, 0.0F,
                     constant * (x2 + y2)};
}

glm::mat3 cubos::engine::ballShapeInertiaTensor(const float mass, const float radius)
{
    float value = (2.0F * mass / 5.0F) * (float)glm::pow(radius, 2);

    return glm::mat3{value, 0.0F, 0.0F, 0.0F, value, 0.0F, 0.0F, 0.0F, value};
}

glm::mat3 cubos::engine::cylinderShapeInertiaTensor(const float mass, const float height, const float radius)
{
    float squareRadius = radius * radius;
    float squareHeight = height * height;
    float offPrincipal = (squareRadius * 3.0F + squareHeight) * mass / 12.0F;

    return glm::mat3{offPrincipal, 0.0F, 0.0F, 0.0F, squareRadius * mass / 2.0F, 0.0F, 0.0F, 0.0F, offPrincipal};
}

glm::mat3 cubos::engine::capsuleShapeInertiaTensor(const float mass, const float length, const float radius)
{
    // Volume of the shape
    float volumeCylinder = cylinderShapeVolume(length, radius);
    float volumeBall = ballShapeVolume(radius);
    float capsuleVolume = volumeCylinder + volumeBall;

    // Inertia Tensor of cilinder and ball that compose this shape
    glm::mat3 cylinderTensor = cylinderShapeInertiaTensor(mass, length, radius);
    glm::mat3 ballTensor = ballShapeInertiaTensor(mass, radius);

    glm::mat3 capsuleTensor = (cylinderTensor * volumeCylinder + ballTensor * volumeBall) * mass / capsuleVolume;

    float extra = (length * length * 0.25F + length * radius * 3.0F / 8.0F) * volumeBall * mass / capsuleVolume;

    capsuleTensor[0][0] += extra;
    capsuleTensor[2][2] += extra;

    return capsuleTensor;
}

// Based on the parallel axis theorem: https://en.wikipedia.org/wiki/Parallel_axis_theorem
glm::mat3 cubos::engine::shiftedInertiaTensor(const glm::mat3& matrix, const glm::vec3& shift, const float mass)
{
    if (mass != 0.0F)
    {
        float diagElement = glm::length2(shift);
        auto diagMatrix = glm::mat3(diagElement);
        auto offsetOuterProduct = glm::mat3(0.0F);
        // glm uses collumn major order
        offsetOuterProduct[0] = shift * shift.x;
        offsetOuterProduct[1] = shift * shift.y;
        offsetOuterProduct[2] = shift * shift.z;

        return matrix + (diagMatrix + offsetOuterProduct) * mass;
    }

    return matrix;
}

// This function computes the inertia tensor for any voxel shape by combining the individual inertia of each voxel
// that composes it. It is based on the idea behind this video: https://youtu.be/CPViCzK7CUo?si=vvJG1Zu5d_kHA-ve, where
// you take each voxel and use it as a point in space. This interpretation allows us to see the inertia as a sum of
// discreet points instead of continuous space. However, in our case, a shape with a single voxel is still a valid voxel
// shape, but would not work correctly since a single point cannot have inertia. This algorithm then takes each voxel,
// as a box shape, computes the inertia for it, around it's own center, and then shifts this inertia matrix relatively
// to the center of mass of the voxel shape, which is assumed to be the origin of local space as usual.
//
// Additionally, although the theory is based on each voxel, it instead uses the boxes stored in the
// VoxelCollisionShape, which reduces the amount of boxes for which the shift needs to be done.
glm::mat3 cubos::engine::voxelShapeInertiaTensor(const float mass, const cubos::engine::VoxelCollisionShape& shape)
{
    auto tensor = glm::mat3(0.0F);
    auto totalShapeVolume = voxelShapeVolume(shape);

    for (auto& box : shape.getBoxes())
    {
        auto subBoxVolume = boxShapeVolume(box.box.halfSize * 2.0F);
        float subBoxMass = mass * subBoxVolume / totalShapeVolume;
        auto subBoxInertia = boxShapeInertiaTensor(subBoxMass, box.box.halfSize * 2.0F);
        // Need to invert the direction shift since it indicates the shift to put the box in the center of mass and we
        // want the contrary.
        tensor += shiftedInertiaTensor(subBoxInertia, -box.shift, subBoxMass);
    }

    return tensor;
}
