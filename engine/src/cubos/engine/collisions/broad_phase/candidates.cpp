#include <cubos/engine/collisions/broad_phase/candidates.hpp>

using namespace cubos::engine;

void BroadPhaseCandidates::addCandidate(CollisionType type, Candidate candidate)
{
    candidatesPerType[static_cast<std::size_t>(type)].insert(candidate);
}

auto BroadPhaseCandidates::candidates(CollisionType type) const -> const std::unordered_set<Candidate, CandidateHash>&
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
