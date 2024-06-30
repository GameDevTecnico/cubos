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

namespace cubos::engine
{
    struct Hit
    {
        Entity entity;
        glm::vec3 point;
    };

    class Raycast
    {
    public:
        Raycast(Query<Entity, const LocalToWorld&, const BoxCollisionShape&, Collider&> boxes,
                Query<Entity, const LocalToWorld&, const CapsuleCollisionShape&, Collider&> spheres)
            : mBoxes{std::move(boxes)}
            , mSpheres{std::move(spheres)} {};

        Hit* fire(glm::vec3 direction, glm::vec3 origin)
        {
            auto* hit = new Hit();
            for (auto [entity, localToWorld, box, collider] : mBoxes)
            {
                // determine if the ray intersects the box
                float scalar = intersects(origin, direction, localToWorld.inverse(), collider.localAABB);
                if (scalar >= 0.0F)
                {
                    hit->entity = entity;
                    hit->point = glm::vec3(origin + scalar * direction);
                    return hit;
                }
            }

            for (auto [entity, localToWorld, sphere, collider] : mSpheres)
            {
                // determine if the ray intersects the sphere
                float scalar = intersects(origin, direction, localToWorld.inverse(), collider.localAABB);
                if (scalar >= 0.0F)
                {
                    hit->entity = entity;
                    hit->point = glm::vec3(origin + scalar * direction);
                    return hit;
                }
            }

            return nullptr;
        }

    private:
        Query<Entity, const LocalToWorld&, const BoxCollisionShape&, Collider&> mBoxes;
        Query<Entity, const LocalToWorld&, const CapsuleCollisionShape&, Collider&> mSpheres;

        // returns the scalar value to determine the point of intersection
        static float intersects(glm::vec3 origin, glm::vec3 direction, const glm::mat4& worldToLocal,
                                cubos::core::geom::AABB aabb)
        {
            glm::vec3 localOrigin = glm::vec3(worldToLocal * glm::vec4(origin, 1.0F));
            glm::vec3 localDirection = glm::vec3(worldToLocal * glm::vec4(direction, 0.0F));

            glm::vec3 max = aabb.max();
            glm::vec3 min = aabb.min();

            float tMinX = (min.x - localOrigin.x) / localDirection.x;
            float tMaxX = (max.x - localOrigin.x) / localDirection.x;
            float tMinY = (min.y - localOrigin.y) / localDirection.y;
            float tMaxY = (max.y - localOrigin.y) / localDirection.y;
            float tMinZ = (min.z - localOrigin.z) / localDirection.z;
            float tMaxZ = (max.z - localOrigin.z) / localDirection.z;

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
    };
} // namespace cubos::engine

namespace cubos::core::ecs
{
    template <>
    class SystemFetcher<engine::Raycast>
    {
    public:
        static inline constexpr bool ConsumesOptions = false;

        SystemFetcher<Query<Entity, const cubos::engine::LocalToWorld&, const cubos::engine::BoxCollisionShape&,
                            cubos::engine::Collider&>>
            boxes;
        SystemFetcher<Query<Entity, const cubos::engine::LocalToWorld&, const cubos::engine::CapsuleCollisionShape&,
                            cubos::engine::Collider&>>
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