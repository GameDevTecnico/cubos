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
        ~SparseRelationTableRegistry();

        /// @brief Constructs.
        SparseRelationTableRegistry();

        /// @brief Stores the ids of tables of a given type.
        class TypeIndex;

        /// @brief Checks if there's a table with the given identifier.
        /// @param id Identifier.
        /// @return Whether it contains the table or not.
        bool contains(SparseRelationTableId id) const;

        /// @brief Returns a reference to the table with the given identifier.
        ///
        /// Creates the table if it doesn't exist already.
        ///
        /// @param id Identifier.
        /// @param type Type registry used to initialize the table, if necessary.
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

        /// @brief Collects new tables which match the given filter.
        /// @param[out] tables Vector to insert the table identifiers into.
        /// @param counter Counter previously returned by this function. Zero should be used for the first call.
        /// @param filter Function which receives a table identifier and returns a boolean.
        /// @return Counter to be passed to this function in a future call.
        std::size_t collect(std::vector<SparseRelationTableId>& tables, std::size_t counter, auto filter)
        {
            for (; counter < mIds.size(); ++counter)
            {
                if (filter(mIds[counter]))
                {
                    tables.emplace_back(mIds[counter]);
                }
            }

            return counter;
        }

    private:
        std::unordered_map<SparseRelationTableId, SparseRelationTable, SparseRelationTableIdHash> mTables;
        std::unordered_map<std::size_t, TypeIndex> mTypeIndices;
        std::vector<SparseRelationTableId> mIds;
        TypeIndex* mEmptyTypeIndex; ///< Used as a placeholder when a type hasn't been registered yet.
    };

    class SparseRelationTableRegistry::TypeIndex
    {
    public:
        /// @brief Inserts a new table identifier into the index.
        /// @param id Table identifier.
        void insert(SparseRelationTableId id);

        /// @brief Returns a reference to a map which maps archetypes to the tables where it is the 'from' archetype.
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

    private:
        std::unordered_map<ArchetypeId, std::vector<SparseRelationTableId>, ArchetypeIdHash>
            mSparseRelationTableIdsByFrom;
        std::unordered_map<ArchetypeId, std::vector<SparseRelationTableId>, ArchetypeIdHash>
            mSparseRelationTableIdsByTo;
    };
} // namespace cubos::core::ecs
