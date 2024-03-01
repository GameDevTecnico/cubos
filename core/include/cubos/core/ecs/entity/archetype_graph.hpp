/// @file
/// @brief Class @ref cubos::core::ecs::ArchetypeGraph.
/// @ingroup core-ecs-entity

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cubos/core/ecs/entity/archetype_id.hpp>
#include <cubos/core/ecs/table/column.hpp>

namespace cubos::core::ecs
{
    /// @brief Stores which column types each archetype holds and the edges which connect them.
    ///
    /// These edges are bidirectional and indicate the addition or removal of column types, depending on the direction
    /// being taken. Neighboring archetypes (which have only one differing column type) are not always connected by an
    /// edge, as they are only generated the first time the traversal is done between those two archetypes.
    ///
    /// In practice, this means that the first time a component is added to an entity of a given archetype, even if the
    /// target archetype already exists, a slow lookup must first be made. We cache the result of the lookup as a new
    /// edge, such that the next time the same component type is added, we just traverse the edge to find the target
    /// archetype's id.
    ///
    /// @ingroup core-ecs-entity
    class CUBOS_CORE_API ArchetypeGraph
    {
    public:
        /// @brief Constructs.
        ArchetypeGraph();

        /// @brief Checks if the given @p archetype contains the column type with the given @p id.
        /// @param archetype Archetype.
        /// @param id Column type identifier.
        /// @return Whether the archetype contains the column type.
        bool contains(ArchetypeId archetype, ColumnId id) const;

        /// @brief Returns an archetype with same identifiers as the given @p source archetype, but
        /// with an extra column type with the given @p id.
        ///
        /// The column type with the given @p id must not already be present in @p source.
        ///
        /// @param source Source archetype.
        /// @param id Extra column type identifier.
        /// @return Target archetype.
        ArchetypeId with(ArchetypeId source, ColumnId id);

        /// @brief Returns an archetype with same identifiers as the given @p source archetype,
        /// except for the column type with the given @p id, which is excluded.
        ///
        /// The given column type with the given @p id must be present in @p source.
        ///
        /// @param source Source archetype.
        /// @param id Excluded column type identifier.
        /// @return Target archetype.
        ArchetypeId without(ArchetypeId source, ColumnId id);

        /// @brief Returns the first column type in the set of column types held by the given @p archetype.
        /// @param archetype Archetype.
        /// @return First column type, or @ref ColumnId::Invalid if the archetype is @ref Empty.
        ColumnId first(ArchetypeId archetype) const;

        /// @brief Returns the next column type in the set of column types held by the given @p archetype.
        /// @param archetype Archetype.
        /// @param id Current column type.
        /// @return Next column type, or @ref ColumnId::Invalid if there is no next column type.
        ColumnId next(ArchetypeId archetype, ColumnId id) const;

        /// @brief Collects all of the archetypes which are supersets of the given archetype.
        ///
        /// Returns the largest identifier checked during the call, which can then be used to only
        /// check new archetypes in later calls.
        ///
        /// @param archetype Base archetype.
        /// @param[out] supersets Set to insert new found archetypes into.
        /// @param seen Maximum previously seen archetype. To be used after the first call to this.
        /// @return Maximum seen archetype.
        std::size_t collect(ArchetypeId archetype, std::vector<ArchetypeId>& supersets, std::size_t seen = 0) const;

    private:
        /// @brief Represents an archetype in the graph.
        struct Node
        {
            std::unordered_set<uint64_t> ids;                ///< Column types held by the archetype.
            std::unordered_map<uint64_t, ArchetypeId> edges; ///< Cached paths to neighboring archetypes.
        };

        std::vector<Node> mNodes;
    };
} // namespace cubos::core::ecs
