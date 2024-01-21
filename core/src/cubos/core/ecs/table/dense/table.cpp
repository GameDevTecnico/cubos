#include <cubos/core/ecs/table/dense/table.hpp>
#include <cubos/core/log.hpp>

using cubos::core::ecs::DenseTable;
using cubos::core::memory::AnyVector;

void DenseTable::addColumn(ColumnId id, const reflection::Type& type)
{
    CUBOS_ASSERT(mEntities.empty());
    mColumns.emplace(id.inner, type);
}

void DenseTable::pushBack(uint32_t index)
{
    bool inserted = mEntityToRow.emplace(index, mEntities.size()).second;
    CUBOS_ASSERT(inserted, "Entity already exists in table");
    mEntities.push_back(index);
}

void DenseTable::swapErase(uint32_t index)
{
    // Get the row of the entity to remove.
    auto it = mEntityToRow.find(index);
    CUBOS_ASSERT(it != mEntityToRow.end(), "Entity doesn't exist in table");
    auto row = it->second;
    mEntityToRow.erase(it);

    if (row + 1 != mEntities.size())
    {
        // If the entity isn't the last one in the table, first swap it with the last one.
        mEntities[row] = mEntities.back();
        mEntityToRow[mEntities[row]] = row;
    }

    mEntities.pop_back();

    for (auto& [id, column] : mColumns)
    {
        column.swapErase(row);
    }
}

void DenseTable::swapMove(uint32_t index, DenseTable& other)
{
    // Get the row of the entity to move.
    auto it = mEntityToRow.find(index);
    CUBOS_ASSERT(it != mEntityToRow.end(), "Entity doesn't exist in table");
    auto row = it->second;
    mEntityToRow.erase(it);

    if (row + 1 != mEntities.size())
    {
        // If the entity isn't the last one in the table, first swap it with the last one.
        mEntities[row] = mEntities.back();
        mEntityToRow[mEntities[row]] = row;
    }

    mEntities.pop_back();

    // Insert the entity into the other table.
    other.pushBack(index);

    // Move the data in columns common to both tables, and remove the data from columns unique to this table.
    for (auto& [id, column] : mColumns)
    {
        if (auto it = other.mColumns.find(id); it != other.mColumns.end())
        {
            auto& otherColumn = it->second;
            otherColumn.pushUninit();
            column.swapMove(row, otherColumn.at(otherColumn.size() - 1));
        }
        else
        {
            column.swapErase(row);
        }
    }
}

std::size_t DenseTable::row(uint32_t index) const
{
    auto it = mEntityToRow.find(index);
    CUBOS_ASSERT(it != mEntityToRow.end(), "Entity doesn't exist in table");
    return it->second;
}

uint32_t DenseTable::entity(std::size_t row) const
{
    CUBOS_ASSERT(row < mEntities.size(), "Row index out of bounds");
    return mEntities[row];
}

std::size_t DenseTable::size() const
{
    for (const auto& [id, col] : mColumns)
    {
        CUBOS_ASSERT(mEntities.size() == col.size());
    }
    return mEntities.size();
}

AnyVector& DenseTable::column(ColumnId id)
{
    auto it = mColumns.find(id.inner);
    CUBOS_ASSERT(it != mColumns.end(), "Column doesn't exist in table");
    return it->second;
}

const AnyVector& DenseTable::column(ColumnId id) const
{
    auto it = mColumns.find(id.inner);
    CUBOS_ASSERT(it != mColumns.end(), "Column doesn't exist in table");
    return it->second;
}

bool DenseTable::contains(ColumnId id) const
{
    return mColumns.contains(id.inner);
}
