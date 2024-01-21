#include <cubos/core/ecs/entity/archetype_graph.hpp>
#include <cubos/core/log.hpp>

using cubos::core::ecs::ArchetypeGraph;
using cubos::core::ecs::ArchetypeId;
using cubos::core::ecs::ColumnId;

static bool isSuperset(const std::unordered_set<uint64_t>& superset, const std::unordered_set<uint64_t>& set)
{
    for (auto id : set)
    {
        if (!superset.contains(id))
        {
            return false;
        }
    }

    return true;
}

ArchetypeGraph::ArchetypeGraph()
{
    mNodes.emplace_back();
}

bool ArchetypeGraph::contains(ArchetypeId archetype, ColumnId id) const
{
    CUBOS_ASSERT(mNodes.size() > archetype.inner);
    return mNodes[archetype.inner].ids.contains(id.inner);
}

ArchetypeId ArchetypeGraph::with(ArchetypeId source, ColumnId id)
{
    CUBOS_ASSERT(mNodes.size() > source.inner);
    CUBOS_ASSERT(!mNodes[source.inner].ids.contains(id.inner));

    if (auto it = mNodes[source.inner].edges.find(id.inner); it != mNodes[source.inner].edges.end())
    {
        // If this path has been made before, we can just follow the existing cached result.
        return it->second;
    }

    for (std::size_t i = 0; i < mNodes.size(); ++i)
    {
        // Find the node which is a superset of source and has exactly one extra identifier, which
        // matches the requested one.
        if (mNodes[i].ids.size() == mNodes[source.inner].ids.size() + 1 && mNodes[i].ids.contains(id.inner) &&
            isSuperset(mNodes[i].ids, mNodes[source.inner].ids))
        {
            // Cache the followed path for future calls.
            mNodes[source.inner].edges.emplace(id.inner, ArchetypeId{.inner = i});
            mNodes[i].edges.emplace(id.inner, source);
            return {.inner = i};
        }
    }

    // There's no such node yet, we'll have to create it.
    auto archetype = mNodes.size();
    mNodes.emplace_back();

    mNodes[archetype].ids = mNodes[source.inner].ids;
    mNodes[archetype].ids.emplace(id.inner);
    mNodes[archetype].edges.emplace(id.inner, source);
    mNodes[source.inner].edges.emplace(id.inner, ArchetypeId{.inner = archetype});
    return {.inner = archetype};
}

ArchetypeId ArchetypeGraph::without(ArchetypeId source, ColumnId id)
{
    CUBOS_ASSERT(mNodes.size() > source.inner);
    CUBOS_ASSERT(mNodes[source.inner].ids.contains(id.inner));

    if (auto it = mNodes[source.inner].edges.find(id.inner); it != mNodes[source.inner].edges.end())
    {
        // If this path has been made before, we can just follow the existing cached result.
        return it->second;
    }

    for (std::size_t i = 0; i < mNodes.size(); ++i)
    {
        // Find the node which is a subset of source and has exactly one less identifier, which
        // matches the requested one.
        if (mNodes[i].ids.size() == mNodes[source.inner].ids.size() - 1 && !mNodes[i].ids.contains(id.inner) &&
            isSuperset(mNodes[source.inner].ids, mNodes[i].ids))
        {
            // Cache the followed path for future calls.
            mNodes[source.inner].edges.emplace(id.inner, ArchetypeId{.inner = i});
            mNodes[i].edges.emplace(id.inner, source);
            return {.inner = i};
        }
    }

    // There's no such node yet, we'll have to create it.
    auto archetype = mNodes.size();
    mNodes.emplace_back();

    mNodes[archetype].ids = mNodes[source.inner].ids;
    mNodes[archetype].ids.erase(id.inner);
    mNodes[archetype].edges.emplace(id.inner, source);
    mNodes[source.inner].edges.emplace(id.inner, ArchetypeId{.inner = archetype});
    return {.inner = archetype};
}

ColumnId ArchetypeGraph::first(ArchetypeId archetype) const
{
    if (mNodes[archetype.inner].ids.empty())
    {
        return ColumnId::Invalid;
    }

    return ColumnId{.inner = *mNodes[archetype.inner].ids.begin()};
}

ColumnId ArchetypeGraph::next(ArchetypeId archetype, ColumnId id) const
{
    auto it = mNodes[archetype.inner].ids.find(id.inner);
    CUBOS_ASSERT(it != mNodes[archetype.inner].ids.end(), "Archetype does not contain column type");

    ++it;

    if (it == mNodes[archetype.inner].ids.end())
    {
        return ColumnId::Invalid;
    }

    return {.inner = *it};
}

std::size_t ArchetypeGraph::collect(ArchetypeId archetype, std::vector<ArchetypeId>& supersets, std::size_t seen)
{
    for (; seen < mNodes.size(); ++seen)
    {
        if (mNodes[seen].ids.size() >= mNodes[archetype.inner].ids.size() &&
            isSuperset(mNodes[seen].ids, mNodes[archetype.inner].ids))
        {
            supersets.emplace_back(ArchetypeId{.inner = seen});
        }
    }

    return seen;
}
