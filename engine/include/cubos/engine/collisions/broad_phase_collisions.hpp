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
        using Candidate = std::pair<Entity, Entity>;

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
        void addCandidate(CollisionType type, Candidate candidate);

        /// @return The collision candidates for a specific collision type.
        /// @param type The collision type.
        const std::vector<Candidate>& getCandidates(CollisionType type) const;

    private:
        /// List of collision candidates for each collision type. The index of the vector is the collision type.
        std::vector<std::vector<Candidate>> mCandidatesPerType{static_cast<size_t>(CollisionType::AMOUNT)};
    };
} // namespace cubos::engine