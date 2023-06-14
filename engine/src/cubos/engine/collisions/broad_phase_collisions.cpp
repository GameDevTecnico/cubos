#include <cubos/engine/collisions/broad_phase_collisions.hpp>

using cubos::engine::BroadPhaseCollisions;

void BroadPhaseCollisions::addCandidate(CollisionType type, Candidate candidate)
{
    mCandidatesPerType[static_cast<size_t>(type)].push_back(candidate);
}

const std::vector<BroadPhaseCollisions::Candidate>& BroadPhaseCollisions::getCandidates(CollisionType type) const
{
    return mCandidatesPerType[static_cast<size_t>(type)];
}