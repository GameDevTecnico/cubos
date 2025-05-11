/// @file
/// @brief Class @ref cubos::core::ecs::SparseRelationTableRegistry.
/// @ingroup core-ecs-table

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cubos/core/ecs/table/sparse_relation/id.hpp>
#include <cubos/core/ecs/table/sparse_relation/table.hpp>

namespace cubos::core::ecs
{
    /// @brief Stores all of the sparse relation tables.
    /// @ingroup core-ecs-table
    class CUBOS_CORE_API SparseRelationTableRegistry
    {
    public:
        /// @brief Stores the ids of tables of a given type.
        class CUBOS_CORE_API TypeIndex
        {
        public:
            /// @brief Inserts a new table identifier into the index.
            /// @param id Table identifier.
            void insert(SparseRelationTableId id);

            /// @brief Removes a table identifier from the index.
            /// @param id Table identifier.
            void erase(SparseRelationTableId id);

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
            std::unordered_map<ArchetypeId, std::unordered_set<SparseRelationTableId, SparseRelationTableIdHash>,
                               ArchetypeIdHash>
                mSparseRelationTableIdsByFrom;
            std::unordered_map<ArchetypeId, std::unordered_set<SparseRelationTableId, SparseRelationTableIdHash>,
                               ArchetypeIdHash>
                mSparseRelationTableIdsByTo;
            int mMaxDepth{0};
        };

        /// @brief Used by queries to track their state of the cache of calls to @ref forEach.
        struct CacheCursor
        {
            std::size_t version{0}; ///< Version of the registry when the cache was created, i.e., how many cleanups
                                    ///< have been performed.
            std::size_t tableCounter{0}; ///< How many tables have they seen on the current version.
        };

        /// @brief Used to iterate over type indices in the registry.
        using Iterator = std::unordered_map<DataTypeId, TypeIndex, DataTypeIdHash>::const_iterator;

        ~SparseRelationTableRegistry();

        /// @brief Constructs.
        SparseRelationTableRegistry();

        /// @name Forbid any kind of copying.
        /// @{
        SparseRelationTableRegistry(const SparseRelationTableRegistry&) = delete;
        SparseRelationTableRegistry& operator=(const SparseRelationTableRegistry&) = delete;
        /// @}

        /// @brief Resets the registry to its initial state.
        ///
        /// Previously returned identifiers become invalid, as they might be reused.
        void reset();

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

        /// @brief Removes all relations of the given type.
        /// @param type Type identifier.
        void erase(DataTypeId type);

        /// @brief Calls the given function for each new table.
        /// @param cursor Cursor used to track the state of the cache.
        /// @param func Function which receives a table identifier.
        /// @return Whether the cache should be cleaned up.
        bool forEach(CacheCursor& cursor, auto func) const
        {
            bool cleanup = cursor.version != mVersion;
            if (cleanup)
            {
                cursor.version = mVersion;
                cursor.tableCounter = 0;
            }

            for (; cursor.tableCounter < mIds.size(); ++cursor.tableCounter)
            {
                func(cleanup, mIds[cursor.tableCounter]);
                cleanup = false;
            }

            return cleanup;
        }

        /// @brief Cleans up the registry, removing all tables which do not hold any entities.
        /// @warning This invalidates all existing queries - their caches must be updated before they can be used
        /// again, i.e., they must call @ref forEach again.
        void cleanUp();

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
        std::size_t mVersion{0}; ///< How many cleanups have been performed.
        std::unordered_map<SparseRelationTableId, SparseRelationTable, SparseRelationTableIdHash> mTables;
        std::unordered_map<DataTypeId, TypeIndex, DataTypeIdHash> mTypeIndices;
        std::vector<SparseRelationTableId> mIds;
        TypeIndex* mEmptyTypeIndex; ///< Used as a placeholder when a type hasn't been registered yet.
    };
} // namespace cubos::core::ecs
