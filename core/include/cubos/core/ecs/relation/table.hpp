/// @file
/// @brief Class @ref cubos::core::ecs::RelationTable.
/// @ingroup core-ecs-relation

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include <cubos/core/memory/any_vector.hpp>

namespace cubos::core::ecs
{
    /// @brief A table which stores relations.
    class RelationTable final
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
        RelationTable(const reflection::Type& relationType);

        /// @brief Adds a relation between the given indices. If it already exists, overwrites it.
        /// @param from From index.
        /// @param to To index.
        /// @param value Relation value to move from.
        void insert(uint32_t from, uint32_t to, const void* value);

        /// @brief Removes a relation between the given indices. If it didn't exist, does nothing.
        /// @param from From index.
        /// @param to To index.
        /// @return Whether the relation existed.
        bool erase(uint32_t from, uint32_t to);

        /// @brief Removes all relations from the given index.
        /// @param from From index.
        void eraseFrom(uint32_t from);

        /// @brief Removes all relations to the given index.
        /// @param to To index.
        void eraseTo(uint32_t to);

        /// @brief Checks whether the given relation exists between the given indices.
        /// @param from From index.
        /// @param to To index.
        /// @return Whether the relation exists.
        bool contains(uint32_t from, uint32_t to) const;

        /// @brief Gets a pointer to the data of the relation with the given indices.
        /// @param from From index.
        /// @param to To index.
        /// @return Address of the data, or 0 if there's no such relation.
        uintptr_t at(uint32_t from, uint32_t to) const;

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

        memory::AnyVector mRelations;
        std::vector<Row> mRows;
        std::unordered_map<uint32_t, List> mFromRows;
        std::unordered_map<uint32_t, List> mToRows;
        std::unordered_map<uint64_t, uint32_t> mPairRows;
    };

    class RelationTable::Iterator final
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
        Iterator(const RelationTable& table, uint32_t row);

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
        RelationTable& mTable;
        uint32_t mRow;
        mutable Output mOutput;
    };

    class RelationTable::View final
    {
    public:
        /// @brief Used to iterate over the relations in a view.
        class Iterator;

        /// @brief Gets an iterator to the first relation of the view.
        /// @return Iterator.
        Iterator begin() const;

        /// @brief Gets an iterator which represents the end of the view.
        /// @return Iterator.
        Iterator end() const;
    };

    class RelationTable::View::Iterator final
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
        Iterator(const RelationTable& table, uint32_t row, bool isFrom);

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
        RelationTable& mTable;
        uint32_t mRow;
        bool mIsFrom;
        mutable Output mOutput;
    };
} // namespace cubos::core::ecs
