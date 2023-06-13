/// @file
/// @brief Contains the class for the BroadPhaseCollisions resource.

#pragma once

#include <vector>

#include <cubos/core/ecs/entity_manager.hpp>

using cubos::core::ecs::Entity;

namespace cubos::engine
{
    /// Used to store data used in broad phase collision detection.
    class BroadPhaseCollisions final
    {
    public:
        using CollisionCandidate = std::pair<Entity, Entity>;

        BroadPhaseCollisions() = default;
        ~BroadPhaseCollisions() = default;

        /// Collision type for each pair of colliders.
        enum class CollisionType
        {
            BoxBox = 0,
            BoxCapsule,
            CapsuleCapsule,
            BoxPlane,
            CapsulePlane,
            PlanePlane,
            BoxSimplex,
            CapsuleSimplex,
            PlaneSimplex,
            SimplexSimplex,

            AMOUNT ///< Used to get the amount of collision types.
        };

        /// Adds a collision candidate to the list of candidates for a specific collision type.
        /// @param type The collision type.
        /// @param candidate The collision candidate.
        inline void addCandidate(CollisionType type, CollisionCandidate candidate)
        {
            mCandidatesPerType[static_cast<size_t>(type)].push_back(candidate);
        }

        /// @return The collision candidates for a specific collision type.
        /// @param type The collision type.
        inline const std::vector<CollisionCandidate>& getCandidates(CollisionType type) const
        {
            return mCandidatesPerType[static_cast<size_t>(type)];
        }

    private:
        /// List of collision candidates for each collision type. The index of the vector is the collision type.
        std::vector<std::vector<CollisionCandidate>> mCandidatesPerType{static_cast<size_t>(CollisionType::AMOUNT)};
    };
} // namespace cubos::engine