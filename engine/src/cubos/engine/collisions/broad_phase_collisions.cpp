#include <cubos/core/log.hpp>

#include <cubos/engine/collisions/broad_phase_collisions.hpp>

using cubos::core::ecs::Entity;

using cubos::engine::BroadPhaseCollisions;

void BroadPhaseCollisions::addEntity(Entity entity)
{
    for (auto& markers : markersPerAxis)
    {
        markers.push_back({entity, true});
        markers.push_back({entity, false});
    }
}

void BroadPhaseCollisions::removeEntity(Entity entity)
{
    for (auto& markers : markersPerAxis)
    {
        markers.erase(std::remove_if(markers.begin(), markers.end(),
                                     [entity](const SweepMarker& m) { return m.entity == entity; }),
                      markers.end());
    }
}

void BroadPhaseCollisions::clearEntities()
{
    for (auto& markers : markersPerAxis)
    {
        markers.clear();
    }
}

void BroadPhaseCollisions::addCandidate(CollisionType type, Candidate candidate)
{
    candidatesPerType[static_cast<std::size_t>(type)].insert(candidate);
}

auto BroadPhaseCollisions::candidates(CollisionType type) const -> const std::unordered_set<Candidate, CandidateHash>&
{
    return candidatesPerType[static_cast<std::size_t>(type)];
}

void BroadPhaseCollisions::clearCandidates()
{
    for (auto& candidates : candidatesPerType)
    {
        candidates.clear();
    }
}
