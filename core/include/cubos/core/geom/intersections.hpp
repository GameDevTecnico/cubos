/// @file
/// @brief Class @ref cubos::core::geom::Intersections.
/// @ingroup core-geom

#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/geom/box.hpp>
#include <cubos/core/geom/plane.hpp>
#include <cubos/core/geom/polygonal_feature.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::geom
{
    /// @brief Contains info regarding an intersection between shapes.
    /// @ingroup core-geom
    struct CUBOS_CORE_API Intersection
    {
        CUBOS_REFLECT;

        float penetration; ///< Penetration depth of the collision. Always positive in the direction of the normal.
        glm::vec3 normal;  ///< Normal of contact on the surface of the entity.
    };

    /// @brief Computes the intersection between two box shapes.
    /// @param box1 Box shape of the first entity.
    /// @param localToWorld1 Local to world matrix of the first entity.
    /// @param box2 Box shape of the second entity.
    /// @param localToWorld2 Local to world matrix of the second entity.
    CUBOS_CORE_API bool intersects(const Box& box1, const glm::mat4& localToWorld1, const Box& box2,
                                   const glm::mat4& localToWorld2, Intersection& intersect);

    /// @brief Performs the Sutherland-Hodgman Clipping algorithm.
    /// @param polygon The polygon to perform the clipping on.
    /// @param numClipPlanes Number of cliping planes.
    /// @param clipPlanes Clipping planes
    /// @param removeNotClipToPlane Whether to remove the points if they're outside the plane.
    CUBOS_CORE_API void sutherlandHodgmanClipping(PolygonalFeature& polygon, int numClipPlanes,
                                                  const cubos::core::geom::Plane* clipPlanes,
                                                  bool removeNotClipToPlane);

    /// @brief Compute the intersection between a plane and an edge.
    /// @param plane The plane.
    /// @param start The start point of the edge.
    /// @param end The end point of the edge.
    /// @param outPoint The position of the intersection.
    /// @return True if there's intersection, false otherwise.
    CUBOS_CORE_API bool planeEdgeIntersection(const cubos::core::geom::Plane& plane, const glm::vec3& start,
                                              const glm::vec3& end, glm::vec3& outPoint);

} // namespace cubos::core::geom
