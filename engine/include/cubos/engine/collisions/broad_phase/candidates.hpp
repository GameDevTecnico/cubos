/// @file
/// @brief Resource @ref cubos::engine::Candidates.
/// @ingroup broad-phase-collisions-plugin

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/ecs/entity/manager.hpp>

namespace cubos::engine
{
    /// @brief Resource which stores candidates found in broad phase collision detection.
    /// @ingroup broad-phase-collisions-plugin
    struct BroadPhaseCandidates
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

        /// @brief Sets of collision candidates for each collision type. The index of the array is
        /// the collision type.
        std::unordered_set<Candidate, CandidateHash> candidatesPerType[static_cast<std::size_t>(CollisionType::Count)];

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
