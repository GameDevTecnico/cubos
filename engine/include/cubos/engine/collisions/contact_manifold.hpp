/// @file
/// @brief Relation @ref cubos::engine::ContactManifold.
/// @ingroup collisions-plugin

#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Indicates the type of a geometric feature (vertex, edge, or face) and the feature index. Currently each
    /// feature index is simply the index in the array with that information in the shape.
    class CUBOS_ENGINE_API ContactFeatureId
    {
    public:
        CUBOS_REFLECT;

        /// @brief Feature id identifying an undefined feature.
        static const uint32_t UNDEFINED = 0x0;

        /// @brief Container of the current feature Id.
        uint32_t id = UNDEFINED;

        /// @brief Assign a vertex feature id.
        /// @param vertexId index of the vertex on the shape.
        void setAsVertex(uint32_t vertexId)
        {
            if ((vertexId & TypeMask) != 0)
            {
                return;
            }
            id = TypeVertex | vertexId;
        }

        /// @brief Assign an edge feature id.
        /// @param edgeId index of the edge on the shape.
        void setAsEdge(uint32_t edgeId)
        {
            if ((edgeId & TypeMask) != 0)
            {
                return;
            }
            id = TypeEdge | edgeId;
        }

        /// @brief Assign a face feature id.
        /// @param faceId index of the face on the shape.
        void setAsFace(uint32_t faceId)
        {
            if ((faceId & TypeMask) != 0)
            {
                return;
            }
            id = TypeFace | faceId;
        }

        /// @brief Check if the feature type is a vertex.
        /// @return true if it identifies a vertex.
        inline bool isVertex() const
        {
            return (id & TypeMask) == TypeVertex;
        }

        /// @brief Check if the feature type is an edge.
        /// @return true if it identifies an edge.
        inline bool isEdge() const
        {
            return (id & TypeMask) == TypeEdge;
        }

        /// @brief Check if the feature type is a face.
        /// @return true if it identifies a face.
        inline bool isFace() const
        {
            return (id & TypeMask) == TypeFace;
        }

        inline constexpr bool operator==(const ContactFeatureId& other) const
        {
            return id != UNDEFINED && other.id != UNDEFINED && id == other.id;
        }

    private:
        /// TODO: reduce size of the id to less bits?
        /// For now our id has 32 bits (2 bits for the feature type, 30 for the feature index). If we need to change it
        /// in the future change the CODE_MASK and the each of the types' header to the adequate size.
        /// Identifies the part of the id that corresponds to the feature index. 0011'1111'1111'1111'1111'1111'1111'1111
        static const std::uint32_t IdMask = 0x3fffffff;
        /// Identifies the part of the id that corresponds to the feature type. 1100'0000'0000'0000'0000'0000'0000'0000
        static const std::uint32_t TypeMask = ~IdMask;
        /// The index corresponding to each feature type.
        static const std::uint32_t TypeVertex = 0x40000000; // 0b01 << 30
        static const std::uint32_t TypeEdge = 0x80000000;   // 0b10 << 30
        static const std::uint32_t TypeFace = 0xC0000000;   // 0b11 << 30
    };

    /// @brief Contains info regarding a contact point of a @ContactManifold.
    struct CUBOS_ENGINE_API ContactPointData
    {
        CUBOS_REFLECT;

        cubos::core::ecs::Entity entity; ///< Entity to which the normal is relative to.
        glm::vec3 globalOn1;             ///< Position on the entity of the contact in global coordinates.
        glm::vec3 globalOn2;             ///< Position on the other entity of the contact in global coordinates.
        glm::vec3 localOn1;              ///< Position on the entity of the contact in local coordinates.
        glm::vec3 localOn2;              ///< Position on the other entity of the contact in local coordinates.
        float penetration;               ///< Penetration of the contact point. Always positive.

        float normalImpulse;
        float frictionImpulse1;
        float frictionImpulse2;
        /// The contact feature ID on the first shape. This indicates the ID of
        /// the vertex, edge, or face of the contact, if one can be determined.
        ContactFeatureId fid1;
        ContactFeatureId fid2;
    };

    /// @brief Represents a contact interface between two bodies.
    struct CUBOS_ENGINE_API ContactManifold
    {
        CUBOS_REFLECT;

        cubos::core::ecs::Entity entity;      ///< Entity to which the normal is relative to.
        glm::vec3 normal;                     ///< A contact normal shared by all contacts in this manifold,
                                              ///< expressed in the local space of the first entity.
        std::vector<ContactPointData> points; ///< Contact points of this manifold.
    };
} // namespace cubos::engine
