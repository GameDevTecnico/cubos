#include <algorithm>
#include <limits>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/geom/intersections.hpp>
#include <cubos/core/geom/utils.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/tel/logging.hpp>

static const float MaxDifference = static_cast<float>(1.0e-7);

using namespace cubos::core::geom;

CUBOS_REFLECT_IMPL(cubos::core::geom::Intersection)
{
    return core::ecs::TypeBuilder<Intersection>("cubos::engine::Intersection")
        .withField("penetration", &Intersection::penetration)
        .withField("normal", &Intersection::normal)
        .build();
}

static void getAxes(const glm::mat4& localToWorld, glm::vec3 axes[])
{
    glm::vec3 normals[3];
    Box::normals(normals);

    for (int i = 0; i < 3; i++)
    {
        axes[i] = glm::normalize(localToWorld * glm::vec4(normals[i], 0.0F));
    }
}

static glm::vec2 project(const Box& box, glm::vec3 axis, const glm::mat4& localToWorld)
{
    glm::vec3 corners[8];
    box.corners(corners);

    double min = glm::dot(axis, glm::vec3(localToWorld * glm::vec4(corners[0], 1.0F)));
    double max = min;
    for (int i = 1; i < 8; i++)
    {
        double p = glm::dot(axis, glm::vec3(localToWorld * glm::vec4(corners[i], 1.0F)));
        if (p < min)
        {
            min = p;
        }
        else if (p > max)
        {
            max = p;
        }
    }
    return glm::vec2{min, max};
}

static bool overlap(glm::vec2 p1, glm::vec2 p2)
{
    return p1.y > p2.x && p2.y > p1.x;
}

static float getOverlap(glm::vec2 p1, glm::vec2 p2)
{
    float o = glm::min(p1.y, p2.y) - glm::max(p1.x, p2.x);

    float l = p1.x - p2.x;
    float r = p1.y - p2.y;
    if (l * r < 0.0F) // is contained
    {
        o += glm::min(glm::abs(l), glm::abs(r));
    }

    return o;
}

static bool intersectsOnAxis(const Box& box1, const glm::mat4& localToWorld1, const Box& box2,
                             const glm::mat4& localToWorld2, glm::vec3& axis, glm::vec3& direction,
                             Intersection& intersect)
{
    glm::vec2 p1 = project(box1, axis, localToWorld1);
    glm::vec2 p2 = project(box2, axis, localToWorld2);

    if (!overlap(p1, p2))
    {
        // then we can guarantee that the shapes do not overlap
        return false;
    }

    // get the overlap
    float o = getOverlap(p1, p2);
    // check for minimum
    if (o < intersect.penetration)
    {
        // then set this one as the smallest
        intersect.penetration = o;
        intersect.normal = glm::dot(direction, axis) < 0.0F ? -axis : axis;
    }

    return true;
}

bool cubos::core::geom::intersects(const Box& box1, const glm::mat4& localToWorld1, const Box& box2,
                                   const glm::mat4& localToWorld2, Intersection& intersect)
{
    glm::vec3 faceAxes1[3];
    glm::vec3 faceAxes2[3];
    getAxes(localToWorld1, faceAxes1);
    getAxes(localToWorld2, faceAxes2);

    // Start the penetration at the highest possible value
    intersect.penetration = std::numeric_limits<float>::infinity();

    // Vector from the first entity's center of mass to the second
    glm::vec3 direction = glm::normalize(glm::vec3(localToWorld2[3]) - glm::vec3(localToWorld1[3]));

    for (glm::vec3 axis : faceAxes1)
    {
        if (!intersectsOnAxis(box1, localToWorld1, box2, localToWorld2, axis, direction, intersect))
        {
            return false;
        }
    }

    for (glm::vec3 axis : faceAxes2)
    {
        if (!intersectsOnAxis(box1, localToWorld1, box2, localToWorld2, axis, direction, intersect))
        {
            return false;
        }
    }

    for (glm::vec3 axis1 : faceAxes1)
    {
        for (glm::vec3 axis2 : faceAxes2)
        {
            glm::vec3 edgeAxis = glm::cross(axis1, axis2);
            if (glm::all(glm::equal(edgeAxis, glm::vec3(0.0F, 0.0F, 0.0F), MaxDifference)))
            {
                continue;
            }
            edgeAxis = glm::normalize(edgeAxis);

            if (!intersectsOnAxis(box1, localToWorld1, box2, localToWorld2, edgeAxis, direction, intersect))
            {
                return false;
            }
        }
    }
    return true;
}

bool cubos::core::geom::intersects(const Frustum& frustum, const AABB& aabb)
{
    auto planes = {&frustum.top, &frustum.right, &frustum.bottom, &frustum.left, &frustum.near, &frustum.far};
    return std::ranges::all_of(planes, [&](const Plane* plane) {
        // Compute the vertex of the AABB that is farthest in the direction of the plane normal.
        glm::vec3 posVert{
            plane->normal.x >= 0.0F ? aabb.max().x : aabb.min().x,
            plane->normal.y >= 0.0F ? aabb.max().y : aabb.min().y,
            plane->normal.z >= 0.0F ? aabb.max().z : aabb.min().z,
        };

        // If that vertex is outside the plane, the AABB is completely outside.
        return pointInPlane(posVert, *plane);
    });
}

bool cubos::core::geom::intersects(const Frustum& frustum, const Box& box, const glm::mat4& localToWorld)
{
    return intersects(frustum, AABB::fromOBB(box, localToWorld));
}

bool cubos::core::geom::intersects(const Frustum& frustum, const Capsule& capsule, const glm::mat4& localToWorld)
{
    // capsule is a sphere
    if (capsule.length == 0.0F)
    {
        glm::vec3 center = localToWorld * glm::vec4{0.0F, 0.0F, 0.0F, 1.0F};
        auto planes = {&frustum.top, &frustum.right, &frustum.bottom, &frustum.left, &frustum.near, &frustum.far};
        return std::ranges::all_of(planes, [center, capsule](const Plane* plane) {
            return pointDistanceToPlane(center, *plane) + capsule.radius >= 0.0F;
        });
    }

    return intersects(frustum, capsule.aabb().box(), localToWorld);
}

void cubos::core::geom::sutherlandHodgmanClipping(PolygonalFeature& polygon, int numClipPlanes,
                                                  const cubos::core::geom::Plane* clipPlanes, bool removeNotClipToPlane)
{
    if (numClipPlanes == 0)
    {
        return;
    }

    // Create temporary list of vertices
    std::vector<glm::vec3> tempPolygon1;
    std::vector<uint32_t> tempPolygonIds1;
    std::vector<glm::vec3> tempPolygon2;
    std::vector<uint32_t> tempPolygonIds2;
    std::vector<glm::vec3>* input = &tempPolygon1;
    std::vector<uint32_t>* inputIds = &tempPolygonIds1;
    std::vector<glm::vec3>* output = &tempPolygon2;
    std::vector<uint32_t>* outputIds = &tempPolygonIds2;

    *input = polygon.vertices;
    *inputIds = polygon.vertexIds;

    // Iterate over each clip plane
    for (int i = 0; i < numClipPlanes; i++)
    {
        // If every point has already been removed previously, just exit
        if (input->empty())
        {
            break;
        }

        const cubos::core::geom::Plane& plane = clipPlanes[i];

        glm::vec3 tempPoint;
        glm::vec3 startPoint = input->back();
        for (size_t p = 0; p < input->size(); p++)
        {
            const glm::vec3& endPoint = (*input)[p];
            bool startInPlane = pointInPlane(startPoint, plane);
            bool endInPlane = pointInPlane(endPoint, plane);

            if (removeNotClipToPlane)
            {
                if (endInPlane)
                {
                    output->push_back(endPoint);
                    outputIds->push_back((*inputIds)[p]);
                }
            }
            else
            {
                if (startInPlane && endInPlane)
                {
                    output->push_back(endPoint);
                    outputIds->push_back((*inputIds)[p]);
                }
                else if (startInPlane && !endInPlane)
                {
                    if (planeEdgeIntersection(plane, startPoint, endPoint, tempPoint))
                    {
                        output->push_back(tempPoint);
                        outputIds->push_back((*inputIds)[p]);
                    }
                }
                else if (!startInPlane && endInPlane)
                {
                    if (planeEdgeIntersection(plane, startPoint, endPoint, tempPoint))
                    {
                        output->push_back(tempPoint);
                        outputIds->push_back((*inputIds)[p]);
                    }

                    output->push_back(endPoint);
                    outputIds->push_back((*inputIds)[p]);
                }
            }

            startPoint = endPoint;
        }

        // Swap input/output polygons, and clear output list to generate next
        std::swap(input, output);
        output->clear();

        std::swap(inputIds, outputIds);
        outputIds->clear();
    }

    polygon.vertices = *input;
    polygon.vertexIds = *inputIds;
}

bool cubos::core::geom::planeEdgeIntersection(const cubos::core::geom::Plane& plane, const glm::vec3& start,
                                              const glm::vec3& end, glm::vec3& outPoint)
{
    glm::vec3 edge = end - start;

    // Check that the edge and plane are not parallel and thus never intersect
    float dotEdge = glm::dot(plane.normal, edge);
    if (glm::abs(dotEdge) < MaxDifference)
    {
        return false;
    }

    // Generate a random point on the plane
    glm::vec3 pointOnPlane = plane.normal * (-plane.d);

    // Work out the edge factor to scale edge by
    //  e.g. how far along the edge to traverse before it meets the plane.
    float factor = -glm::dot(plane.normal, start - pointOnPlane) / dotEdge;

    // Stop any large floating point divide issues with almost parallel planes
    factor = glm::clamp(factor, 0.0F, 1.0F);

    // Return point on edge
    outPoint = start + edge * factor;
    return true;
}
