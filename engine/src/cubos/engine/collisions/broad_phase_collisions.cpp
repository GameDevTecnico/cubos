#include <cubos/core/log.hpp>

#include <cubos/engine/collisions/broad_phase_collisions.hpp>

using cubos::engine::BroadPhaseCollisions;

using Candidate = BroadPhaseCollisions::Candidate;
using CandidateHash = BroadPhaseCollisions::CandidateHash;
using CollisionType = BroadPhaseCollisions::CollisionType;
using SweepMarker = BroadPhaseCollisions::SweepMarker;

void BroadPhaseCollisions::addEntity(Entity entity)
{
    for (std::size_t axis = 0; axis < 3; axis++)
    {
        markersPerAxis[axis].push_back({entity, true});
        markersPerAxis[axis].push_back({entity, false});
    }
}

void BroadPhaseCollisions::removeEntity(Entity entity)
{
    for (std::size_t axis = 0; axis < 3; axis++)
    {
        auto& markers = markersPerAxis[axis];
        markers.erase(std::remove_if(markers.begin(), markers.end(),
                                     [entity](const SweepMarker& m) { return m.entity == entity; }),
                      markers.end());
    }
}

void BroadPhaseCollisions::clearEntities()
{
    for (std::size_t axis = 0; axis < 3; axis++)
    {
        markersPerAxis[axis].clear();
    }
}

void BroadPhaseCollisions::addCandidate(CollisionType type, Candidate candidate)
{
    candidatesPerType[static_cast<std::size_t>(type)].insert(candidate);
}

const std::unordered_set<Candidate, CandidateHash>& BroadPhaseCollisions::candidates(CollisionType type) const
{
    return candidatesPerType[static_cast<std::size_t>(type)];
}

void BroadPhaseCollisions::clearCandidates()
{
    for (std::size_t i = 0; i < static_cast<std::size_t>(CollisionType::Count); i++)
    {
        candidatesPerType[i].clear();
    }
}
