/// @file
/// @brief Component @ref cubos::engine::Inertia.
/// @ingroup physics-plugin

#pragma once

#include <glm/gtx/quaternion.hpp>
#include <glm/mat3x3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>
#include <cubos/engine/collisions/shapes/voxel.hpp>
#include <cubos/engine/voxels/grid.hpp>

namespace cubos::engine
{
    /// @brief Component which defines the inertia of a body.
    ///
    /// The tensor of inertia gives us how the mass is distributed in a rigid body, representing resistance to angular
    /// acceleration. A higher angular inertia requires more torque for the same acceleration.
    ///
    /// It does not take into account child entities, even when set to update automatically.
    ///
    /// A total angular inertia of zero is a special case, and is interpreted as infinite inertia, meaning the rigid
    /// body will not be affected by any torque.
    ///
    /// For bodies that are symmetric in the usual x, y and z axis, the tensor is always diagonal.
    /// For a general three-dimensional body, such as any voxel shape, it may not be. However, it is always possible to
    /// find 3 mutually orthogonal axis (an x', y', z' coordinate system) for which the products of inertia are zero,
    /// and the inertia matrix takes a diagonal form. A more thourough explanation can be found here:
    /// https://ocw.mit.edu/courses/16-07-dynamics-fall-2009/dd277ec654440f4c2b5b07d6c286c3fd_MIT16_07F09_Lec26.pdf
    /// @ingroup physics-plugin
    struct CUBOS_ENGINE_API Inertia
    {
        CUBOS_REFLECT;

        static constexpr glm::mat3 INFINITE =
            glm::mat3(std::numeric_limits<float>::infinity(), 0.0F, 0.0F, 0.0F, std::numeric_limits<float>::infinity(),
                      0.0F, 0.0F, 0.0F, std::numeric_limits<float>::infinity());

        /// @brief The inertia tensor in local space.
        glm::mat3 inertia;

        /// @brief The inverse inertia tensor in local space.
        glm::mat3 inverseInertia;

        /// @brief Whether to update the component automatically when the @ref Mass or the collision shape change.
        bool autoUpdate = false;

        /// @brief Sets the inertia from the given inertia tensor.
        /// @param inertiaTensor
        void setFromTensor(glm::mat3 inertiaTensor);

        /// @brief Sets the inertia from the principal inertia and the principal local frame.
        /// @param principalInertia The diagonal of a diagonal inertia tensor.
        /// @param principalLocalFrame The orientation of the principal inertial axis, for which the inertia tensor is
        /// diagonal.
        void setWithLocalFrame(glm::vec3 principalInertia, glm::quat principalLocalFrame);

        /// @brief Computes the inertia tensor with the given rotation. Can be used to transform the local inertia
        /// tensor to world space.
        /// @param rotationQuat Quarternion with the desired rotation.
        /// @return The inertia tensor rotated.
        glm::mat3 rotatedInertia(const glm::quat& rotationQuat) const;

        // TODO: check if we use this many times. If we do, it might be worth it to calculate and store them
        // somewhere instead.
        /// @brief Computes the inverse inertia tensor with the given rotation. Can be used to transform the local
        /// inverse inertia tensor to world space.
        /// @param rotationQuat Quarternion with the desired rotation.
        /// @return The inertia tensor rotated.
        glm::mat3 rotatedInverseInertia(const glm::quat& rotationQuat) const;
    };

    /// @brief Compute Inertia Tensor for box shape. The shape is symmetric so it guarantees a diagonal inertia tensor.
    /// @param mass of the body.
    /// @param dimensions of the shape.
    /// @return Inertia Tensor for a box shape.
    glm::mat3 CUBOS_ENGINE_API boxShapeInertiaTensor(float mass, glm::vec3 dimensions);

    /// @brief Compute Inertia Tensor for ball shape. The shape is symmetric so it guarantees a diagonal inertia tensor.
    /// @param mass of the body.
    /// @param radius of the shape.
    /// @return Inertia tensor for a ball shape.
    glm::mat3 CUBOS_ENGINE_API ballShapeInertiaTensor(float mass, float radius);

    /// @brief Compute Inertia Tensor for capsule shape. The shape is symmetric so it guarantees a diagonal inertia
    /// tensor.
    /// @param mass of the body.
    /// @param dimensions of the shape
    /// @return Inertia Tensor for a cylinder shape.
    glm::mat3 CUBOS_ENGINE_API cylinderShapeInertiaTensor(float mass, float height, float radius);

    /// @brief Compute Inertia Tensor for cylinder shape. The shape is symmetric so it guarantees a diagonal inertia
    /// tensor.
    /// @param mass of the body.
    /// @param length of the shape. Corresponds to just the height of the cylinder part.
    /// @param radius of the shape.
    /// @return Inertia tensor for a capsule shape.
    glm::mat3 CUBOS_ENGINE_API capsuleShapeInertiaTensor(float mass, float length, float radius);

    /// @brief Compute Inertia Tensor for a voxel shape. In case the shape is not symmetric, the inertia will not be
    /// diagonal.
    /// @param mass Mass of the body.
    /// @param shape Voxel collision shape.
    /// @return Inertia tensor for a voxel shape.
    glm::mat3 CUBOS_ENGINE_API voxelShapeInertiaTensor(float mass, const VoxelCollisionShape& shape);

    /// @brief Computes the inertia tensor shifted by the given vector, taking into account the given mass.
    /// @param matrix The inertia tensor.
    /// @param shift The offset to shift the inertia tensor.
    /// @param mass Mass to consider for the shift.
    /// @return Inertia tensor shifted by the given offset.
    glm::mat3 CUBOS_ENGINE_API shiftedInertiaTensor(const glm::mat3& matrix, const glm::vec3& shift, const float mass);
} // namespace cubos::engine
