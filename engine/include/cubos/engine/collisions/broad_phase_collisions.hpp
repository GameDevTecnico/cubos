/// @file
/// @brief Resource @ref cubos::engine::BroadPhaseCollisions.
/// @ingroup collisions-plugin

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/ecs/entity/manager.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores data used in broad phase collision detection.
    /// @ingroup collisions-plugin
    struct BroadPhaseCollisions
    {
        /// @brief Pair of entities that may collide.
        using Candidate = std::pair<core::ecs::Entity, core::ecs::Entity>;

        /// @brief Hash function to allow Candidates to be used as keys in an unordered_set.
        struct CandidateHash
        {
            std::size_t operator()(const Candidate& candidate) const
            {
                return core::ecs::EntityHash()(candidate.first) ^ core::ecs::EntityHash()(candidate.second);
            }
        };

        /// @brief Collision type for each pair of colliders.
        enum class CollisionType
        {
            BoxBox = 0,
            BoxCapsule,
            CapsuleCapsule,

            Count ///< Number of collision types.
        };

        /// @brief Marker used for sweep and prune.
        struct SweepMarker
        {
            core::ecs::Entity entity; ///< Entity referenced by the marker.
            bool isMin;               ///< Whether the marker is a min or max marker.
        };

        /// @brief List of ordered sweep markers for each axis. Stores the index of the marker in mMarkers.
        std::vector<SweepMarker> markersPerAxis[3];

        /// @brief Maps of overlapping entities for each axis calculated by sweep and prune.
        ///
        /// For each each map, the key is an entity and the value is a list of entities that
        /// overlap with the key. Symmetrical pairs are not stored.
        std::unordered_map<core::ecs::Entity, std::vector<core::ecs::Entity>, core::ecs::EntityHash>
            sweepOverlapMaps[3];

        /// @brief Set of active entities during sweep for each axis.
        std::unordered_set<core::ecs::Entity, core::ecs::EntityHash> activePerAxis[3];

        /// @brief Sets of collision candidates for each collision type. The index of the array is
        /// the collision type.
        std::unordered_set<Candidate, CandidateHash> candidatesPerType[static_cast<std::size_t>(CollisionType::Count)];

        /// @brief Adds an entity to the list of entities tracked by sweep and prune.
        /// @param entity Entity to add.
        void addEntity(core::ecs::Entity entity);

        /// @brief Removes an entity from the list of entities tracked by sweep and prune.
        /// @param entity Entity to remove.
        void removeEntity(core::ecs::Entity entity);

        /// @brief Clears the list of entities tracked by sweep and prune.
        void clearEntities();

        /// @brief Adds a collision candidate to the list of candidates for a specific collision type.
        /// @param type Collision type.
        /// @param candidate Collision candidate.
        void addCandidate(CollisionType type, Candidate candidate);

        /// @brief Gets the collision candidates for a specific collision type.
        /// @param type Collision type.
        /// @return Collision candidates.
        const std::unordered_set<Candidate, CandidateHash>& candidates(CollisionType type) const;

        /// @brief Clears the list of collision candidates.
        void clearCandidates();
    };
} // namespace cubos::engine
