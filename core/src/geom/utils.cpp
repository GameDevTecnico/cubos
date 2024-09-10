#include <algorithm>
#include <limits>

#include <glm/glm.hpp>
#include <glm/mat3x3.hpp>

#include <cubos/core/geom/utils.hpp>

bool cubos::core::geom::pointInPlane(const glm::vec3& point, const cubos::core::geom::Plane& plane)
{
    return glm::dot(point, plane.normal) + plane.d >= 0.0F;
}

int cubos::core::geom::getMaxVertexInAxis(const int numVertices, const glm::vec3 vertices[], const glm::vec3& localAxis)
{
    float maxValue = glm::dot(localAxis, vertices[0]);
    int maxVertex = 0;

    float current;
    for (int i = 1; i < numVertices; ++i)
    {
        current = glm::dot(localAxis, vertices[i]);

        if (current > maxValue)
        {
            maxValue = current;
            maxVertex = i;
        }
    }

    return maxVertex;
}

void cubos::core::geom::getIncidentReferencePolygon(const cubos::core::geom::Box& shape, const glm::vec3& normal,
                                                    std::vector<glm::vec3>& outPoints, glm::vec3& outNormal,
                                                    std::vector<cubos::core::geom::Plane>& outAdjacentPlanes,
                                                    const glm::mat4& localToWorld, float scale)
{
    glm::mat3 m = glm::mat3(localToWorld) / scale;
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(m));
    glm::mat3 invNormalMatrix = glm::inverse(normalMatrix);

    // Normal in local coordinates
    glm::vec3 localNormal = invNormalMatrix * normal;

    glm::vec3 vertices[8];
    shape.corners(vertices);

    // Get the furthest vertex along axis - this will be part of the furthest face
    const int vertex = getMaxVertexInAxis(8, vertices, localNormal);

    // Compute which face (that contains the furthest vertex above)
    // is the furthest along the given axis. This is defined by
    // it's normal being closest to parallel with the collision axis.
    glm::vec3 faceNormals[6];
    cubos::core::geom::Box::faceNormals(faceNormals);

    int enclosingFacesIndices[3];
    cubos::core::geom::Box::enclosingFaces(vertex, enclosingFacesIndices);

    int bestFaceIndex = 0;
    float highestCorrelation = -std::numeric_limits<float>::infinity();
    for (int faceIndex : enclosingFacesIndices)
    {
        float tempCorrelation = glm::dot(localNormal, faceNormals[faceIndex]);
        if (tempCorrelation > highestCorrelation)
        {
            highestCorrelation = tempCorrelation;
            bestFaceIndex = faceIndex;
        }
    }

    // Output face normal
    outNormal = glm::normalize(normalMatrix * faceNormals[bestFaceIndex]);

    glm::ivec4 faces[6];
    cubos::core::geom::Box::faces(faces);

    // Output face vertices (transformed back into world-space)
    for (int i = 0; i < 4; i++)
    {
        int vertexIndex = faces[bestFaceIndex][i];
        outPoints.emplace_back(localToWorld * glm::vec4(vertices[vertexIndex], 1.0F));
    }

    // Loop over all adjacent faces and output a clip plane for each.
    glm::ivec2 edgesIndices[12];
    cubos::core::geom::Box::edges(edgesIndices);

    int faceEdgesIndices[4];
    cubos::core::geom::Box::faceEdges(bestFaceIndex, faceEdgesIndices);

    for (int edgeIndex : faceEdgesIndices)
    {
        int vertexIndex = edgesIndices[edgeIndex][0];
        glm::vec3 wsPointOnPlane = glm::vec3(localToWorld * glm::vec4(vertices[vertexIndex], 1.0F));

        int adjacentEdgeFacesIndices[2];
        cubos::core::geom::Box::edgeAdjacentFaces(edgeIndex, adjacentEdgeFacesIndices);

        for (int faceIndex : adjacentEdgeFacesIndices)
        {
            if (faceIndex != bestFaceIndex)
            {
                glm::vec3 planeNormal = glm::normalize(-(normalMatrix * faceNormals[faceIndex]));
                float planeDistance = -glm::dot(planeNormal, wsPointOnPlane);

                auto plane = Plane{};
                plane.normal = planeNormal;
                plane.d = planeDistance;
                outAdjacentPlanes.push_back(plane);
            }
        }
    }
}

glm::vec3 cubos::core::geom::getClosestPointEdge(const glm::vec3& point, glm::vec3 start, glm::vec3 end)
{
    glm::vec3 startToPoint = point - start;
    glm::vec3 startToEnd = end - start;

    // Distance along the line of point in world distance
    float productSPSE = glm::dot(startToPoint, startToEnd);
    float magnitude = glm::dot(startToEnd, startToEnd);

    // Distance along the line of point between 0-1 where 0 is line start and 1 is line end.
    float distance = productSPSE / magnitude;

    // Clamp distance so it cant go beyond the line's start/end in either direction
    distance = glm::clamp(distance, 0.0F, 1.0F);

    return start + startToEnd * distance;
}

glm::vec3 cubos::core::geom::getClosestPointPolygon(const glm::vec3& point, const std::vector<glm::vec3>& polygon)
{
    auto finalClosestPoint = glm::vec3(0.0F);
    float finalClosestDistanceSqr = std::numeric_limits<float>::infinity();

    glm::vec3 last = polygon.back();
    for (const glm::vec3& next : polygon)
    {
        glm::vec3 edgeClosestPoint = getClosestPointEdge(point, last, next);

        // Compute the distance of the closest point on the line to the actual point
        glm::vec3 diff = point - edgeClosestPoint;
        float tempDistanceSqr = glm::dot(diff, diff);

        if (tempDistanceSqr < finalClosestDistanceSqr)
        {
            finalClosestDistanceSqr = tempDistanceSqr;
            finalClosestPoint = edgeClosestPoint;
        }

        last = next;
    }

    return finalClosestPoint;
}

void cubos::core::geom::getCameraFrustumCorners(const glm::mat4& view, const glm::mat4& proj, float zNear, float zFar,
                                                std::vector<glm::vec4>& corners)
{
    // Convert zNear and zFar to normalized coordinates.
    // If they don't match the clipping planes of the projection matrix, the points returned
    // won't be the actual corners of the frustum. This may be useful for splitting the
    // frustum along clipping planes between near and far.

    // Points are converted from view space to clip space
    auto pointNear = proj * glm::vec4(0.0F, 0.0F, -zNear, 1.0F);
    auto pointFar = proj * glm::vec4(0.0F, 0.0F, -zFar, 1.0F);
    pointNear /= pointNear.w;
    pointFar /= pointFar.w;

    float zNearNDC = std::clamp(pointNear.z, -1.0F, 1.0F);
    float zFarNDC = std::clamp(pointFar.z, -1.0F, 1.0F);

    // Proceed to getting the corners

    auto viewProjInv = glm::inverse(proj * view);

    for (int x = -1; x <= 1; x += 2)
    {
        for (int y = -1; y <= 1; y += 2)
        {
            auto cornerNear = viewProjInv * glm::vec4(x, y, zNearNDC, 1.0F);
            corners.push_back(cornerNear / cornerNear.w);

            auto cornerFar = viewProjInv * glm::vec4(x, y, zFarNDC, 1.0F);
            corners.push_back(cornerFar / cornerFar.w);
        }
    }
}
