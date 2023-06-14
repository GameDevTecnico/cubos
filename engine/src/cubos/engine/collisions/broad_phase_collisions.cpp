#include <unordered_set>

#include <cubos/engine/collisions/broad_phase_collisions.hpp>

using cubos::engine::BroadPhaseCollisions;

template <typename F>
void BroadPhaseCollisions::updateMarkers(F position)
{
    for (auto& entity : mEntities)
    {
        auto [min, max] = bounds(entity.entity);
        entity.min = min;
        entity.max = max;
    }

    for (size_t axis = 0; axis < 3; axis++)
    {
        std::sort(mMarkersPerAxis[axis].begin(), mMarkersPerAxis[axis].end(),
                  [this, axis](const SweepMarker& a, const SweepMarker& b) {
                      auto a = mEntities[a.entityIndex];
                      auto b = mEntities[b.entityIndex];
                      return (a.isMin ? a.min : a.max)[axis] < (b.isMin ? b.min : b.max)[axis];
                  });
    }
}

const std::vector<std::unordered_map<Entity, std::vector<Entity>>>& BroadPhaseCollisions::sweep()
{
    mOverlapMaps.clear();
    auto active = std::unordered_set<Entity>{};

    for (size_t axis = 0; axis < 3; axis++)
    {
        for (auto& marker : mMarkersPerAxis[axis])
        {
            auto& entity = mEntities[marker.entityIndex];

            if (marker.isMin)
            {
                for (auto& other : active)
                {
                    mOverlapMaps[axis][entity.entity].push_back(other);
                }

                active.insert(entity.entity);
            }
            else
            {
                active.erase(entity.entity);
            }
        }
    }

    return mOverlapMaps;
}

void BroadPhaseCollisions::addEntity(Entity entity)
{
    mEntities.push_back({entity, glm::vec3(0.0f)});

    for (size_t axis = 0; axis < 3; axis++)
    {
        mMarkersPerAxis[axis].push_back({mEntities.size() - 1, true});
        mMarkersPerAxis[axis].push_back({mEntities.size() - 1, false});
    }
}

void BroadPhaseCollisions::removeEntity(Entity entity)
{
    auto it =
        std::find_if(mEntities.begin(), mEntities.end(), [entity](const SweepEntity& e) { return e.entity == entity; });

    if (it != mEntities.end())
    {
        auto index = std::distance(mEntities.begin(), it);
        mEntities.erase(it);

        for (size_t axis = 0; axis < 3; axis++)
        {
            auto& markers = mMarkersPerAxis[axis];
            markers.erase(std::remove_if(markers.begin(), markers.end(),
                                         [index](const SweepMarker& m) { return m.entityIndex == index; }),
                          markers.end());
        }
    }
}

void BroadPhaseCollisions::clearEntities()
{
    mEntities.clear();
    for (size_t axis = 0; axis < 3; axis++)
    {
        mMarkersPerAxis[axis].clear();
    }
}

void BroadPhaseCollisions::addCandidate(CollisionType type, Candidate candidate)
{
    mCandidatesPerType[static_cast<size_t>(type)].push_back(candidate);
}

const std::vector<BroadPhaseCollisions::Candidate>& BroadPhaseCollisions::getCandidates(CollisionType type) const
{
    return mCandidatesPerType[static_cast<size_t>(type)];
}