/// @file
/// @brief Class @ref cubos::core::ecs::QueryRelatedNode.
/// @ingroup core-ecs-query

#pragma once

#include <vector>

#include <cubos/core/ecs/query/node/archetype.hpp>
#include <cubos/core/ecs/query/term.hpp>
#include <cubos/core/ecs/table/sparse_relation/id.hpp>

namespace cubos::core::ecs
{
    /// @brief Node which forces two given targets to be related with a given relation.
    class QueryRelatedNode final : public QueryNode
    {
    public:
        /// @brief Constructs.
        /// @param cursor Cursor index.
        /// @param dataType Relation data type.
        /// @param isSymmetric Whether the relation is symmetric.
        /// @param includeDuplicates Whether, if the relation is symmetric, a relation may appear twice.
        /// @param traversal Traversal type, in case we're filtering a tree relation.
        /// @param fromNode From target node.
        /// @param toNode To target node.
        QueryRelatedNode(int cursor, DataTypeId dataType, bool isSymmetric, bool includeDuplicates, Traversal traversal,
                         QueryArchetypeNode& fromNode, QueryArchetypeNode& toNode);

        std::size_t estimate() const override;
        void update(World& world) override;
        bool next(World& world, TargetMask pins, Iterator& iterator) const override;

    private:
        const DataTypeId mDataType;
        const bool mIsSymmetric;
        const bool mIncludeDuplicates;
        const Traversal mTraversal;
        QueryArchetypeNode& mFromNode;
        QueryArchetypeNode& mToNode;

        /// @brief Entry in the reverse table cache.
        struct ReverseEntry
        {
            SparseRelationTableId id; ///< Table identifier.
            bool isDuplicate;         ///< Whether the table is also present in the mTables array.
        };

        /// @brief Tables which match the requirements, maintained through calls to @ref update.
        std::vector<SparseRelationTableId> mTables;

        /// @brief Tables which match the requirements in the reverse order, maintained through calls to @ref update.
        ///
        /// Only kept if the relation is symmetric.
        std::vector<ReverseEntry> mReverseTables;

        /// @brief How many tables have already been seen through @ref SparseRelationTableRegistry::collect.
        std::size_t mSeenCount{0};
    };
} // namespace cubos::core::ecs
