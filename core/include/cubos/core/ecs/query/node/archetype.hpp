/// @file
/// @brief Class @ref cubos::core::ecs::QueryArchetypeNode.
/// @ingroup core-ecs-query

#pragma once

#include <vector>

#include <cubos/core/ecs/query/node/node.hpp>
#include <cubos/core/ecs/table/column.hpp>

namespace cubos::core::ecs
{
    /// @brief Node which forces a given target to belong to a set of archetypes.
    class QueryArchetypeNode final : public QueryNode
    {
    public:
        /// @brief Constructs.
        /// @param target Target.
        QueryArchetypeNode(int target);

        /// @brief Forces the node to match only archetypes with the given column.
        void with(ColumnId column);

        /// @brief Forces the node to match only archetypes without the given column.
        void without(ColumnId column);

        /// @brief Returns whether two nodes are equivalent.
        /// @param other Other node.
        bool equivalent(const QueryArchetypeNode& other) const;

        /// @brief Gets the target being filtered.
        int target() const;

        /// @brief Gets the known archetypes which match the requirements for the target.
        const std::vector<ArchetypeId>& archetypes() const;

        std::size_t estimate() const override;
        void update(World& world) override;
        bool next(World& world, TargetMask pins, Iterator& iterator) const override;

    private:
        std::vector<ColumnId> mWith;
        std::vector<ColumnId> mWithout;

        /// @brief Archetype which only and all of the columns in mWith.
        ArchetypeId mBaseArchetype{ArchetypeId::Invalid};

        /// @brief Archetypes which match the requirements, maintained through calls to @ref update.
        std::vector<ArchetypeId> mArchetypes;

        /// @brief How many archetypes have already been seen through @ref ArchetypeGraph::collect.
        std::size_t mSeenCount{0};
    };
} // namespace cubos::core::ecs
