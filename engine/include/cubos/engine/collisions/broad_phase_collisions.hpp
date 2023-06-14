/// @file
/// @brief Contains the class for the BroadPhaseCollisions resource.

#pragma once

#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

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

            Count ///< Used to get the amount of collision types.
        };

        /// Updates and sorts the markers for sweep and prune.
        /// @param position Function used to get the bounds of the entity.
        template <typename F>
        void updateMarkers(F position);

        /// Sweeps the markers for each axis and adds overlapping entities to the overlap map of the axis.
        /// @returns The overlap maps.
        const std::vector<std::unordered_map<Entity, std::vector<Entity>>>& sweep();

        /// Adds an entity to the list of entities tracked by sweep and prune.
        /// An updateMarkers() call is required after adding entities.
        /// @param entity The entity to add.
        void addEntity(Entity entity);

        /// Removes an entity from the list of entities tracked by sweep and prune.
        /// @param entity The entity to remove.
        void removeEntity(Entity entity);

        /// Clears the list of entities tracked by sweep and prune.
        void clearEntities();

        /// Adds a collision candidate to the list of candidates for a specific collision type.
        /// @param type The collision type.
        /// @param candidate The collision candidate.
        void addCandidate(CollisionType type, Candidate candidate);

        /// @return The collision candidates for a specific collision type.
        /// @param type The collision type.
        const std::vector<Candidate>& getCandidates(CollisionType type) const;

    private:
        /// Entity tracked by sweep and prune.
        struct SweepEntity
        {
            Entity entity; ///< The entity.
            glm::vec3 min; ///< The minimum position of the entity.
            glm::vec3 max; ///< The maximum position of the entity.
        };

        /// Marker used for sweep and prune.
        struct SweepMarker
        {
            size_t entityIndex; ///< Index of the entity in mEntities.
            bool isMin;         ///< Whether the marker is a min or max marker.
        };

        /// List of entities tracked by sweep and prune.
        std::vector<SweepEntity> mEntities;

        /// List of ordered sweep markers for each axis. Stores the index of the marker in mMarkers.
        std::vector<std::vector<SweepMarker>> mMarkersPerAxis{3};

        /// Maps of overlapping entities for each axis calculated by sweep and prune.
        /// For each each map, the key is an entity and the value is a list of entities that overlap with the key.
        /// Symmetrical pairs are not stored.
        std::vector<std::unordered_map<Entity, std::vector<Entity>>> mSweepOverlapMaps;

        /// List of collision candidates for each collision type. The index of the vector is the collision type.
        std::vector<std::vector<Candidate>> candidatesPerType{static_cast<size_t>(CollisionType::Count)};
    };
} // namespace cubos::engine
