#include <cubos/engine/collisions/broad_phase/candidates.hpp>

using namespace cubos::engine;

using Candidate = BroadPhaseCandidates::Candidate;
using CandidateHash = BroadPhaseCandidates::CandidateHash;

void BroadPhaseCandidates::addCandidate(CollisionType type, Candidate candidate)
{
    candidatesPerType[static_cast<std::size_t>(type)].insert(candidate);
}

const std::unordered_set<Candidate, CandidateHash>& BroadPhaseCandidates::candidates(CollisionType type) const
{
    return candidatesPerType[static_cast<std::size_t>(type)];
}

void BroadPhaseCandidates::clearCandidates()
{
    for (auto& candidates : candidatesPerType)
    {
        candidates.clear();
    }
}
