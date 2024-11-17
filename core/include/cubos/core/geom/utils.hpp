/// @file
/// @brief Class @ref cubos::core::geom::Utils.
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
    /// @brief Computes if a point is in the area limited by the plane
    /// @param point The position of the point in world coordinates.
    /// @param plane The plane to use.
    /// @return True is the point is on the plane or closer, false otherwise.
    CUBOS_CORE_API bool pointInPlane(const glm::vec3& point, const cubos::core::geom::Plane& plane);

    /// @brief Computes the vertex furthest along the axis.
    /// @param vertices Array of vertices.
    /// @param localAxis The direction of the axis in local coordinates.
    /// @return The index of the vertex.
    CUBOS_CORE_API int getMaxVertexInAxis(int numVertices, const glm::vec3 vertices[], const glm::vec3& localAxis);

    /// TODO: since this is a box shape, we know how many points and adjacent planes exist beforehand
    /// @brief Computes the candidate face of the polygon to be reference, as well as it's normal and adjacent planes.
    /// @param shape Collider shape.
    /// @param normal The reference normal in world coordinates.
    /// @param outPolygon The polygon of the resulting face.
    /// @param outAdjacentPlanes The resulting adjacent planes (4).
    /// @param outAdjacentPlanesIds The resulting adjacent planes ids (4).
    /// @param localToWorld The localToWorld matrix of the body.
    /// @param scale The scale of the body.
    CUBOS_CORE_API void getIncidentReferencePolygon(const cubos::core::geom::Box& shape, const glm::vec3& normal,
                                                    PolygonalFeature& outPolygon,
                                                    std::vector<cubos::core::geom::Plane>& outAdjacentPlanes,
                                                    std::vector<uint32_t> outAdjacentPlanesIds,
                                                    const glm::mat4& localToWorld, float scale);

    /// @brief Computes the closest point on the line (edge) to point.
    /// @param point Position of the point.
    /// @param start Starting position of the edge.
    /// @param end End position of the edge.
    /// @return Closest position on edge to the point.
    CUBOS_CORE_API glm::vec3 getClosestPointEdge(const glm::vec3& point, glm::vec3 start, glm::vec3 end);

    /// @brief Iterates through all edges in polygon and computes closest position on the edges of the polygon to point.
    /// @param point Position of the point.
    /// @param polygon List of points that compose the shape (edges are defined as a line-loop list of vertices).
    /// @return Closest point that resides on any of the given edges of the polygon.
    CUBOS_CORE_API glm::vec3 getClosestPointPolygon(const glm::vec3& point, const std::vector<glm::vec3>& polygon);

    /// @brief Gets a camera's frustum corners in world space.
    /// @param view Matrix that transforms world space to the camera's view space.
    /// @param proj Matrix that transforms the camera's view space to its clip space.
    /// @param zNear Near clipping plane.
    /// @param zFar Far clipping plane.
    /// @param corners Output vector where the corners will be stored.
    CUBOS_CORE_API void getCameraFrustumCorners(const glm::mat4& view, const glm::mat4& proj, float zNear, float zFar,
                                                std::vector<glm::vec4>& corners);

    /// @brief Gets view matrices for rendering a cubemap.
    /// @param inverseView Matrix that transforms the camera's view space to world space.
    /// @param cubeViewMatrices Output vector where the view matrices will be stored.
    CUBOS_CORE_API void getCubeViewMatrices(const glm::mat4& inverseView, std::vector<glm::mat4>& cubeViewMatrices);
} // namespace cubos::core::geom
