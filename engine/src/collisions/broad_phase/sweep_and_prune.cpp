#include <algorithm>

#include <cubos/core/ecs/reflection.hpp>

#include "sweep_and_prune.hpp"

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(BroadPhaseSweepAndPrune)
{
    return core::ecs::TypeBuilder<BroadPhaseSweepAndPrune>("cubos::engine::BroadPhaseSweepAndPrune").build();
}

void BroadPhaseSweepAndPrune::addEntity(Entity entity)
{
    for (auto& markers : markersPerAxis)
    {
        markers.push_back({entity, true});
        markers.push_back({entity, false});
    }
}

void BroadPhaseSweepAndPrune::removeEntity(Entity entity)
{
    for (auto& markers : markersPerAxis)
    {
        markers.erase(std::remove_if(markers.begin(), markers.end(),
                                     [entity](const SweepMarker& m) { return m.entity == entity; }),
                      markers.end());
    }
}

void BroadPhaseSweepAndPrune::clearEntities()
{
    for (auto& markers : markersPerAxis)
    {
        markers.clear();
    }
}
