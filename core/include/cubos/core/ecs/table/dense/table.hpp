/// @file
/// @brief Class @ref cubos::core::ecs::DenseTable.
/// @ingroup core-ecs-table

#pragma once

#include <unordered_map>
#include <vector>

#include <cubos/core/ecs/table/column.hpp>
#include <cubos/core/memory/any_vector.hpp>

namespace cubos::core::ecs
{
    /// @brief Stores the dense data associated to entities of a given archetype.
    /// @ingroup core-ecs-table
    class CUBOS_CORE_API DenseTable final
    {
    public:
        /// @brief Constructs a table without columns.
        DenseTable() = default;

        /// @name Forbid any kind of copying.
        /// @{
        DenseTable(const DenseTable&) = delete;
        DenseTable& operator=(const DenseTable&) = delete;
        /// @}

        /// @brief Adds a new column to the table.
        ///
        /// The table must still be empty, and the column must not already exist in the table.
        ///
        /// @param id Column type identifier.
        /// @param type Column data type.
        void addColumn(ColumnId id, const reflection::Type& type);

        /// @brief Inserts a new entity to the end of the table.
        /// @warning The table will be temporarily left in an invalid state, and the caller must manually push data to
        /// each column before the table can be used again.
        /// @param index Entity index.
        void pushBack(uint32_t index);

        /// @brief Removes an entity from the table.
        ///
        /// If the entity isn't the last one in the table, the last entity will be moved to the position of the removed
        /// entity. The data columns will be updated accordingly.
        ///
        /// @param index Entity index.
        void swapErase(uint32_t index);

        /// @brief Moves an entity from this table into another table, effectively calling @ref pushBack() on the
        /// other table, and @ref swapErase() on this one.
        /// @warning While data columns common to both tables will be updated by this function, any data columns
        /// unique to the other table will be left in an invalid state, and the caller must manually push data to each
        /// column before the destination table can be used again.
        /// @param index Entity index.
        /// @param other Destination table.
        void swapMove(uint32_t index, DenseTable& other);

        /// @brief Gets the row which contains the data for the given entity.
        /// @param index Entity index.
        /// @return Row index.
        std::size_t row(uint32_t index) const;

        /// @brief Gets the index of the entity which is stored in the given row.
        /// @param row Row index.
        /// @return Entity index.
        uint32_t entity(std::size_t row) const;

        /// @brief Gets the number of rows in the table.
        /// @return Number of rows.
        std::size_t size() const;

        /// @brief Gets a reference to the column with the given @p id.
        ///
        /// Aborts if the column does not exist in the table.
        ///
        /// @param id Column identifier.
        /// @return Column reference.
        memory::AnyVector& column(ColumnId id);

        /// @copydoc column(ColumnId)
        const memory::AnyVector& column(ColumnId id) const;

        /// @brief Checks if the table has a column with the given @p id.
        /// @param id Column identifier.
        /// @return Whether the table contains the given column.
        bool contains(ColumnId id) const;

    private:
        std::vector<uint32_t> mEntities;
        std::unordered_map<uint32_t, std::size_t> mEntityToRow;
        std::unordered_map<uint64_t, memory::AnyVector> mColumns;
    };
} // namespace cubos::core::ecs
