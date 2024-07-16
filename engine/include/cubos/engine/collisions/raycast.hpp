/// @file
/// @brief System argument @ref cubos::engine::Raycast.
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
    /// @brief System argument which allows performing raycast queries among all entities with colliders.
    ///
    /// An example of using this system argument:
    ///
    /// @code{.cpp}
    ///     cubos.system("do raycast")
    ///     .call([](Raycast raycast) {
    ///         if (auto hit = raycast.fire({origin, direction}))
    ///         {
    ///             CUBOS_INFO("Hit {} at {}", hit->entity, hit->point);
    ///         }
    ///     });
    /// @endcode
    ///
    /// @ingroup collisions-plugin
    class CUBOS_ENGINE_API Raycast
    {
    public:
        /// @brief Holds the result of a raycast query.
        struct Hit
        {
            Entity entity;   ///< Entity that was hit.
            glm::vec3 point; ///< Point of intersection.
        };

        /// @brief Describes the ray used in a raycast query.
        struct Ray
        {
            glm::vec3 origin;    ///< Origin of the ray.
            glm::vec3 direction; ///< Direction of the ray.
        };

        /// @brief Constructs.
        /// @param boxes Query for entities with box colliders.
        /// @param capsules Query for entities with capsule colliders.
        Raycast(Query<Entity, const LocalToWorld&, const BoxCollisionShape&> boxes,
                Query<Entity, const LocalToWorld&, const CapsuleCollisionShape&, const Position&> capsules)
            : mBoxes{std::move(boxes)}
            , mCapsules{std::move(capsules)} {};

        /// @brief Fires a ray and returns the first hit.
        /// @param ray Ray to fire.
        /// @return Hit if a hit was found, otherwise nullopt.
        Opt<Hit> fire(Ray ray);

    private:
        Query<Entity, const LocalToWorld&, const BoxCollisionShape&> mBoxes;
        Query<Entity, const LocalToWorld&, const CapsuleCollisionShape&, const Position&> mCapsules;
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
            capsules;

        SystemFetcher(World& world, const SystemOptions& options)
            : boxes{world, options}
            , capsules{world, options}
        {
        }

        void analyze(SystemAccess& access) const
        {
            boxes.analyze(access);
            capsules.analyze(access);
        }

        cubos::engine::Raycast fetch(const SystemContext& ctx)
        {
            return {boxes.fetch(ctx), capsules.fetch(ctx)};
        }
    };
} // namespace cubos::core::ecs
