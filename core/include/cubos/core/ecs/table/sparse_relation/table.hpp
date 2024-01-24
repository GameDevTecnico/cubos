/// @file
/// @brief Class @ref cubos::core::ecs::SparseRelationTable.
/// @ingroup core-ecs-relation

#pragma once

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include <cubos/core/memory/any_vector.hpp>

namespace cubos::core::ecs
{
    /// @brief A table which stores relations. Allows for quick insertion, deletion and iteration.
    ///
    /// Internally, the table is stored simply as a vector of relations and their data. Each row in
    /// this table stores:
    /// - the 'from' entity index;
    /// - the 'to' entity index;
    /// - the previous and next rows with the same 'from' index;
    /// - the previous and next rows with the same 'to' index;
    /// - the relation data itself.
    ///
    /// To make random accesses more efficient, we also store an hashtable which maps entity index
    /// pairs to rows. This way, we can quickly check if a relation exists or where it's stored.
    ///
    /// Additionally, we store two other hashtables, one which associate 'from' and 'to' indices to
    /// rows in the table which represent the first and last nodes of linked lists, where each node
    /// is a row with the same 'from' or 'to' index, depending on the list.
    ///
    /// These linked lists are essential to provide fast query times, as instead of having to
    /// iterate over the entire table and filter for entity, we only need to follow the linked
    /// list chain.
    class SparseRelationTable final
    {
    public:
        /// @brief Used to iterate over the relations in a table.
        class Iterator;

        /// @brief Used to view part of a table.
        class View;

        /// @brief Constructs with the given relation type.
        /// @note @p relationType must have @ref reflection::ConstructibleTrait and be move
        /// constructible.
        /// @param relationType Relation type.
        SparseRelationTable(const reflection::Type& relationType);

        /// @brief Adds a relation between the given indices. If it already exists, overwrites it.
        /// @param from From index.
        /// @param to To index.
        /// @param value Relation value to move from.
        /// @return Whether the relation already existed.
        bool insert(uint32_t from, uint32_t to, void* value);

        /// @brief Removes a relation between the given indices. If it didn't exist, does nothing.
        /// @param from From index.
        /// @param to To index.
        /// @return Whether the relation existed.
        bool erase(uint32_t from, uint32_t to);

        /// @brief Removes all relations from the given index.
        /// @param from From index.
        /// @return How many relations were erased.
        std::size_t eraseFrom(uint32_t from);

        /// @brief Moves all relations with the given from index to another table.
        /// @param from From index.
        /// @param other Other table. Must have the same relation type.
        /// @return How many relations were erased.
        std::size_t moveFrom(uint32_t from, SparseRelationTable& other);

        /// @brief Removes all relations to the given index.
        /// @param to To index.
        /// @return How many relations were moved.
        std::size_t eraseTo(uint32_t to);

        /// @brief Moves all relations with the given to index to another table.
        /// @param to To index index.
        /// @param other Other table. Must have the same relation type.
        /// @return How many relations were moved.
        std::size_t moveTo(uint32_t to, SparseRelationTable& other);

        /// @brief Checks whether the given relation exists between the given indices.
        /// @param from From index.
        /// @param to To index.
        /// @return Whether the relation exists.
        bool contains(uint32_t from, uint32_t to) const;

        /// @brief Gets the row of the relation with the given indices, or @ref size() if there is
        /// none.
        /// @param from From index.
        /// @param to To index.
        /// @return Row of the data.
        std::size_t row(uint32_t from, uint32_t to) const;

        /// @brief Get the relation at the given row.
        /// @note Aborts if @p row is out of bounds.
        /// @param row Row of the relation to get.
        /// @return Pointer to the relation.
        void* at(std::size_t row);

        /// @copydoc at(std::size_t)
        const void* at(std::size_t row) const;

        /// @brief Get the entity indices at the given row.
        /// @note Aborts if @p row is out of bounds.
        /// @param row Row to get.
        /// @param[out] from From index.
        /// @param[out] to To index.
        void indices(std::size_t row, uint32_t& from, uint32_t& to) const;

        /// @brief Gets the first row with the given from index, or @ref size() if there is none.
        /// @param index From index.
        /// @return First row with the given from index.
        std::size_t firstFrom(uint32_t index) const;

        /// @brief Gets the first row with the given to index, or @ref size() if there is none.
        /// @param index To index.
        /// @return First row with the given to index.
        std::size_t firstTo(uint32_t index) const;

        /// @brief Gets the next row with the same from index, or @ref size() if there is none.
        /// @param row Row to get the next from.
        /// @return Next row with the same from index.
        std::size_t nextFrom(std::size_t row) const;

        /// @brief Gets the next row with the same to index, or @ref size() if there is none.
        /// @param row Row to get the next to.
        /// @return Next row with the same to index.
        std::size_t nextTo(std::size_t row) const;

        /// @brief Gets an iterator to the first relation of the table.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator which represents the end of the table.
        /// @return Iterator.
        Iterator end() const;

        /// @brief Returns a view of the relations with the given from index.
        /// @param from From index.
        /// @return Relation view.
        View viewFrom(uint32_t from) const;

        /// @brief Returns a view of the relations with the given to index.
        /// @param to To index.
        /// @return Relation view.
        View viewTo(uint32_t to) const;

        /// @brief Returns the number of relations on the table.
        /// @return Relation count.
        std::size_t size() const;

    private:
        /// @brief Link for @ref List.
        struct Link
        {
            uint32_t prev;
            uint32_t next;
        };

        /// @brief List of rows with the same from/to index.
        struct List
        {
            uint32_t first;
            uint32_t last;
        };

        /// @brief Stores metadata for the relations.
        struct Row
        {
            uint32_t from;
            uint32_t to;
            Link fromLink;
            Link toLink;
        };

        /// @brief Appends the row with the given index to the end of both linked lists.
        /// @param index Row index.
        void appendLink(uint32_t index);

        /// @brief Removes references to the row with the given index in both linked lists.
        /// @param index Row index.
        void eraseLink(uint32_t index);

        /// @brief Updates the links of the given row such that its neighbors point to it correctly.
        /// @param index Row index.
        void updateLink(uint32_t index);

        memory::AnyVector mRelations;
        const reflection::ConstructibleTrait* mConstructibleTrait{nullptr};
        std::vector<Row> mRows;
        std::unordered_map<uint32_t, List> mFromRows;
        std::unordered_map<uint32_t, List> mToRows;
        std::unordered_map<uint64_t, uint32_t> mPairRows;
    };

    class SparseRelationTable::Iterator final
    {
    public:
        /// @brief Output structure for the iterator.
        struct Output
        {
            uint32_t from;   ///< From index.
            uint32_t to;     ///< To index.
            uintptr_t value; ///< Address of the relation.
        };

        /// @brief Constructs.
        /// @param table Table.
        /// @param row Row.
        Iterator(const SparseRelationTable& table, uint32_t row);

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same relation.
        bool operator==(const Iterator& other) const;

        /// @brief Accesses the relation referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Output.
        const Output& operator*() const;

        /// @brief Accesses the relation referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Output.
        const Output* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        const SparseRelationTable& mTable;
        uint32_t mRow;
        mutable Output mOutput;
    };

    class SparseRelationTable::View final
    {
    public:
        /// @brief Used to iterate over the relations in a view.
        class Iterator;

        /// @brief Constructs.
        /// @param index Index being iterated over.
        /// @param isFrom Is the index a from index? If false, its a to index.
        View(const SparseRelationTable& table, uint32_t index, bool isFrom);

        /// @brief Gets an iterator to the first relation of the view.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator which represents the end of the view.
        /// @return Iterator.
        Iterator end() const;

    private:
        const SparseRelationTable& mTable;
        uint32_t mIndex;
        bool mIsFrom;
    };

    class SparseRelationTable::View::Iterator final
    {
    public:
        /// @brief Output structure for the iterator.
        struct Output
        {
            uint32_t from;   ///< From index.
            uint32_t to;     ///< To index.
            uintptr_t value; ///< Address of the relation.
        };

        /// @brief Constructs.
        /// @param table Table.
        /// @param row Row.
        /// @param isFrom Are we iterating over the same from index? If false, iterates over to.
        Iterator(const SparseRelationTable& table, uint32_t row, bool isFrom);

        /// @brief Copy constructs.
        /// @param other Other iterator.
        Iterator(const Iterator& other) = default;

        /// @brief Compares two iterators.
        /// @param other Other iterator.
        /// @return Whether the iterators point to the same relation.
        bool operator==(const Iterator& other) const;

        /// @brief Accesses the relation referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Output.
        const Output& operator*() const;

        /// @brief Accesses the relation referenced by this iterator.
        /// @note Aborts if out of bounds.
        /// @return Output.
        const Output* operator->() const;

        /// @brief Advances the iterator.
        /// @note Aborts if out of bounds.
        /// @return Reference to this.
        Iterator& operator++();

    private:
        const SparseRelationTable& mTable;
        uint32_t mRow;
        bool mIsFrom;
        mutable Output mOutput;
    };
} // namespace cubos::core::ecs
