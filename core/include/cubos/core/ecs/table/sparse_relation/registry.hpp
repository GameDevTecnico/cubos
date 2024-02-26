/// @file
/// @brief Class @ref cubos::core::ecs::SparseRelationTableRegistry.
/// @ingroup core-ecs-table

#pragma once

#include <unordered_map>
#include <vector>

#include <cubos/core/ecs/table/sparse_relation/id.hpp>
#include <cubos/core/ecs/table/sparse_relation/table.hpp>

namespace cubos::core::ecs
{
    /// @brief Stores all of the sparse relation tables.
    /// @ingroup core-ecs-table
    class SparseRelationTableRegistry
    {
    public:
        /// @brief Stores the ids of tables of a given type.
        class TypeIndex
        {
        public:
            /// @brief Inserts a new table identifier into the index.
            /// @param id Table identifier.
            void insert(SparseRelationTableId id);

            /// @brief Returns a reference to a map which maps archetypes to the tables where it is the 'from'
            /// archetype.
            /// @return Map from archetypes to vectors of table identifiers.
            const auto& from() const
            {
                return mSparseRelationTableIdsByFrom;
            }

            /// @brief Returns a reference to a map which maps archetypes to the tables where it is the 'to' archetype.
            /// @return Map from archetypes to vectors of table identifiers.
            const auto& to() const
            {
                return mSparseRelationTableIdsByTo;
            }

            /// @brief Returns the maximum depth of the tables in this index.
            /// @return Maximum depth.
            int maxDepth() const
            {
                return mMaxDepth;
            }

        private:
            std::unordered_map<ArchetypeId, std::vector<SparseRelationTableId>, ArchetypeIdHash>
                mSparseRelationTableIdsByFrom;
            std::unordered_map<ArchetypeId, std::vector<SparseRelationTableId>, ArchetypeIdHash>
                mSparseRelationTableIdsByTo;
            int mMaxDepth{0};
        };

        /// @brief Used to iterate over type indices in the registry.
        using Iterator = std::unordered_map<DataTypeId, TypeIndex, DataTypeIdHash>::const_iterator;

        ~SparseRelationTableRegistry();

        /// @brief Constructs.
        SparseRelationTableRegistry();

        /// @brief Checks if there's a table with the given identifier.
        /// @param id Identifier.
        /// @return Whether it contains the table or not.
        bool contains(SparseRelationTableId id) const;

        /// @brief Returns a reference to the table with the given identifier.
        ///
        /// Creates the table if it doesn't exist already.
        ///
        /// @param id Identifier.
        /// @param types Type registry used to initialize the table, if necessary.
        /// @return Reference to table.
        SparseRelationTable& create(SparseRelationTableId id, Types& types);

        /// @brief Returns a reference to the table with the given identifier.
        ///
        /// Aborts if the table doesn't exist.
        ///
        /// @param id Identifier.
        /// @return Reference to table.
        SparseRelationTable& at(SparseRelationTableId id);

        /// @copydoc at()
        const SparseRelationTable& at(SparseRelationTableId id) const;

        /// @brief Returns the index used to search for tables of relations with a given type.
        /// @param type Type identifier.
        /// @return Type index.
        const TypeIndex& type(DataTypeId type) const;

        /// @brief Moves all relations of an entity from an archetype to another.
        /// @param source Old archetype.
        /// @param target New archetype.
        /// @param index Entity index.
        void move(ArchetypeId source, ArchetypeId target, uint32_t index);

        /// @brief Removes all relations of an entity.
        /// @param archetype Entity archetype.
        /// @param index Entity index.
        void erase(ArchetypeId archetype, uint32_t index);

        /// @brief Calls the given function for each new table.
        /// @param counter Counter previously returned by this function. Zero should be used for the first call.
        /// @param func Function which receives a table identifier.
        /// @return Counter to be passed to this function in a future call.
        std::size_t forEach(std::size_t counter, auto func) const
        {
            for (; counter < mIds.size(); ++counter)
            {
                func(mIds[counter]);
            }

            return counter;
        }

        /// @brief Gets an iterator to the start of the type indices of this registry.
        /// @return Iterator.
        Iterator begin() const
        {
            return mTypeIndices.cbegin();
        }

        /// @brief Gets an iterator to the end of the type indices of this registry.
        /// @return Iterator.
        Iterator end() const
        {
            return mTypeIndices.cend();
        }

    private:
        std::unordered_map<SparseRelationTableId, SparseRelationTable, SparseRelationTableIdHash> mTables;
        std::unordered_map<DataTypeId, TypeIndex, DataTypeIdHash> mTypeIndices;
        std::vector<SparseRelationTableId> mIds;
        TypeIndex* mEmptyTypeIndex; ///< Used as a placeholder when a type hasn't been registered yet.
    };
} // namespace cubos::core::ecs
