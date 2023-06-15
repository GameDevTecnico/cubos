#include <cubos/engine/collisions/broad_phase_collisions.hpp>

using cubos::engine::BroadPhaseCollisions;
using Candidate = cubos::engine::BroadPhaseCollisions::Candidate;
using CollisionType = cubos::engine::BroadPhaseCollisions::CollisionType;
using SweepMarker = cubos::engine::BroadPhaseCollisions::SweepMarker;

void BroadPhaseCollisions::addEntity(Entity entity)
{
    for (size_t axis = 0; axis < 3; axis++)
    {
        markersPerAxis[axis].push_back({entity, true});
        markersPerAxis[axis].push_back({entity, false});
    }
}

void BroadPhaseCollisions::removeEntity(Entity entity)
{
    for (size_t axis = 0; axis < 3; axis++)
    {
        auto& markers = markersPerAxis[axis];
        markers.erase(std::remove_if(markers.begin(), markers.end(),
                                     [entity](const SweepMarker& m) { return m.entity == entity; }),
                      markers.end());
    }
}

void BroadPhaseCollisions::clearEntities()
{
    for (size_t axis = 0; axis < 3; axis++)
    {
        markersPerAxis[axis].clear();
    }
}

void BroadPhaseCollisions::addCandidate(CollisionType type, Candidate candidate)
{
    candidatesPerType[static_cast<size_t>(type)].push_back(candidate);
}

const std::vector<Candidate>& BroadPhaseCollisions::getCandidates(CollisionType type)
{
    return candidatesPerType[static_cast<size_t>(type)];
}

// template <typename F>
// void BroadPhaseCollisions::updateMarkers(F position)
// {
//     for (auto& entity : entities)
//     {
//         auto [min, max] = bounds(entity.entity);
//         entity.min = min;
//         entity.max = max;
//     }

//     for (size_t axis = 0; axis < 3; axis++)
//     {
//         std::sort(markersPerAxis[axis].begin(), markersPerAxis[axis].end(),
//                   [this, axis](const SweepMarker& a, const SweepMarker& b) {
//                       auto a = entities[a.entityIndex];
//                       auto b = entities[b.entityIndex];
//                       return (a.isMin ? a.min : a.max)[axis] < (b.isMin ? b.min : b.max)[axis];
//                   });
//     }
// }

// const std::vector<std::unordered_map<Entity, std::vector<Entity>>>& BroadPhaseCollisions::sweep()
// {
//     mSweepOverlapMaps.clear();
//     auto active = std::unordered_set<size_t>{};

//     for (size_t axis = 0; axis < 3; axis++)
//     {
//         for (auto& marker : markersPerAxis[axis])
//         {
//             if (marker.isMin)
//             {
//                 auto entity = entities[marker.entityIndex].entity;
//                 for (auto& otherIndex : active)
//                 {
//                     auto other = entities[otherIndex].entity;
//                     mSweepOverlapMaps[axis][entity].push_back(other);
//                 }

//                 active.insert(marker.entityIndex);
//             }
//             else
//             {
//                 active.erase(marker.entityIndex);
//             }
//         }
//     }

//     return mSweepOverlapMaps;
// }
