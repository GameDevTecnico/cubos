/// @file
/// @brief Component @ref cubos::engine::Raycast.
/// @ingroup collisions-plugin

#pragma once

#include <utility>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>

#include <cubos/core/ecs/system/fetcher.hpp>
#include <cubos/core/geom/box.hpp>

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/position.hpp>

namespace cubos::engine
{
    struct Hit
    {
        Entity entity;
        glm::vec3 point;
    };

    struct Ray
    {
        glm::vec3 origin, direction;
    };

    class Raycast
    {
    public:
        Raycast(Query<Entity, const LocalToWorld&, const BoxCollisionShape&> boxes,
                Query<Entity, const LocalToWorld&, const CapsuleCollisionShape&, const Position&> spheres)
            : mBoxes{std::move(boxes)}
            , mSpheres{std::move(spheres)} {};

        Opt<Hit> fire(Ray ray)
        {
            Opt<Hit> hitMax;
            float minScalar = std::numeric_limits<float>::infinity();

            // normalize the ray
            ray.direction = glm::normalize(ray.direction);

            for (auto [entity, localToWorld, shape] : mBoxes)
            {
                auto worldToLocal = localToWorld.inverse();
                Ray localRay = {glm::vec3(worldToLocal * glm::vec4(ray.origin, 1.0F)),
                                glm::normalize(glm::vec3(worldToLocal * glm::vec4(ray.direction, 0.0F)))};
                float scalar = intersects(localRay, shape.box);

                if (scalar <= minScalar && scalar >= 0.0F)
                {
                    Hit hit;
                    hit.entity = entity;
                    hit.point = glm::vec3(ray.origin + scalar * ray.direction);
                    hitMax.replace(hit);
                    minScalar = scalar;
                }
            }

            for (auto [entity, localToWorld, shape, position] : mSpheres)
            {
                auto worldToLocal = localToWorld.inverse();
                Ray localRay = {glm::vec3(worldToLocal * glm::vec4(ray.origin, 1.0F)),
                                glm::normalize(glm::vec3(worldToLocal * glm::vec4(ray.direction, 0.0F)))};
                float scalar = intersects(localRay, worldToLocal, shape.capsule, position);
                if (scalar <= minScalar && scalar >= 0.0F)
                {
                    Hit hit;
                    hit.entity = entity;
                    hit.point = glm::vec3(ray.origin + scalar * ray.direction);
                    hitMax.replace(hit);
                    minScalar = scalar;
                }
            }

            return hitMax;
        }

    private:
        Query<Entity, const LocalToWorld&, const BoxCollisionShape&> mBoxes;
        Query<Entity, const LocalToWorld&, const CapsuleCollisionShape&, const Position&> mSpheres;

        static float intersects(Ray ray, cubos::core::geom::Box box)
        {
            glm::vec3 corners[2];

            box.diag(corners);

            glm::vec3 max = corners[1];
            glm::vec3 min = corners[0];

            float tMinX = (min.x - ray.origin.x) / ray.direction.x;
            float tMaxX = (max.x - ray.origin.x) / ray.direction.x;
            float tMinY = (min.y - ray.origin.y) / ray.direction.y;
            float tMaxY = (max.y - ray.origin.y) / ray.direction.y;
            float tMinZ = (min.z - ray.origin.z) / ray.direction.z;
            float tMaxZ = (max.z - ray.origin.z) / ray.direction.z;

            // find the maximum of the min
            float tMin = std::max(std::max(std::min(tMinX, tMaxX), std::min(tMinY, tMaxY)), std::min(tMinZ, tMaxZ));

            // find the minimum of the max
            float tMax = std::min(std::min(std::max(tMinX, tMaxX), std::max(tMinY, tMaxY)), std::max(tMinZ, tMaxZ));

            if (tMax < 0 || tMin > tMax)
            {
                return -1.0F;
            }

            return tMin < 0.0F ? tMax : tMin;
        }

        static float intersects(Ray ray, float radius, float top, float bottom)
        {
            // We are gonna use the quadratic equation made by subbing the ray equation into the cylinder equation
            // The cylinder equation is:
            // x^2 + z^2 = r^2
            // The ray equation is:
            // x = x0 + t * dx
            // z = z0 + t * dz

            float a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z;
            float b = 2.0F * (ray.direction.x * ray.origin.x + ray.direction.z * ray.origin.z);
            float c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - radius * radius;

            float discriminant = b * b - 4.0F * a * c;
            if (discriminant < 0)
            {
                return -1.0F; // no intersection with the cylinder
            }

            float t1 = (-b + std::sqrt(discriminant)) / (2.0F * a);
            float t2 = (-b - std::sqrt(discriminant)) / (2.0F * a);

            float max = std::max(t1, t2);
            float min = std::min(t1, t2);

            float t = min > 0.0F ? min : max;

            if (t < 0.0F)
            {
                return -1.0F; // no valid intersection
            }

            float y = ray.origin.y + t * ray.direction.y;

            if (y < bottom || y > top)
            {
                return -1.0F; // intersection is outside the finite cylinder
            }

            return t;
        };

        static float intersects(Ray ray, float radius, glm::vec3 center)
        {
            glm::vec3 oc = ray.origin - center;
            float a = glm::dot(ray.direction, ray.direction);
            float b = 2.0F * glm::dot(oc, ray.direction);
            float c = glm::dot(oc, oc) - radius * radius;
            float discriminant = b * b - 4 * a * c;

            if (discriminant < 0)
            {
                return 0.0F;
            }

            float sqrtDiscriminant = std::sqrt(discriminant);
            float t1 = (-b - sqrtDiscriminant) / (2.0F * a);
            float t2 = (-b + sqrtDiscriminant) / (2.0F * a);

            return (t1 >= 0) ? t1 : t2;
        };

        static float intersects(Ray ray, const glm::mat4& worldToLocal, cubos::core::geom::Capsule capsule,
                                Position position)
        {
            float radius = capsule.radius;
            float length = capsule.length;

            if (length == 0.0F)
            {
                // sphere
                return intersects(ray, radius, glm::vec3(worldToLocal * glm::vec4(position.vec, 1.0F)));
            }

            // both centers of the circles limiting the cylinder
            glm::vec3 topCenter = position.vec + glm::vec3(0.0F, length / 2.0F, 0.0F);
            glm::vec3 bottomCenter = position.vec - glm::vec3(0.0F, length / 2.0F, 0.0F);

            glm::vec3 localTopCenter = glm::vec3(worldToLocal * glm::vec4(topCenter, 1.0F));
            glm::vec3 localBottomCenter = glm::vec3(worldToLocal * glm::vec4(bottomCenter, 1.0F));

            float cylinderIntersect = intersects(ray, radius, topCenter.y, bottomCenter.y);
            if (cylinderIntersect > 0)
            {
                return cylinderIntersect;
            }

            float sphereIntersect = intersects(ray, radius, localTopCenter);
            if (sphereIntersect > 0)
            {
                return sphereIntersect;
            }

            sphereIntersect = intersects(ray, radius, localBottomCenter);
            if (sphereIntersect > 0)
            {
                return sphereIntersect;
            }

            return -1.0F;
        }
    };
} // namespace cubos::engine

namespace cubos::core::ecs
{
    template <>
    class SystemFetcher<engine::Raycast>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher<Query<Entity, const cubos::engine::LocalToWorld&, const cubos::engine::BoxCollisionShape&>> boxes;
        SystemFetcher<Query<Entity, const cubos::engine::LocalToWorld&, const cubos::engine::CapsuleCollisionShape&,
                            const cubos::engine::Position&>>
            spheres;

        SystemFetcher(World& world, const SystemOptions& options)
            : boxes{world, options}
            , spheres{world, options}
        {
        }

        void analyze(SystemAccess& access) const
        {
            boxes.analyze(access);
            spheres.analyze(access);
        }

        cubos::engine::Raycast fetch(const SystemContext& ctx)
        {
            return {boxes.fetch(ctx), spheres.fetch(ctx)};
        }
    };
} // namespace cubos::core::ecs