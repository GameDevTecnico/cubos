#include <cubos/core/ecs/table/sparse_relation/table.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::ecs::SparseRelationTable;

/// @brief Creates a single integer which uniquely identifies the relation with the given indices.
/// @param from From index.
/// @param to To index.
/// @return Identifier.
static uint64_t pairId(uint32_t from, uint32_t to)
{
    return static_cast<uint64_t>(from) | (static_cast<uint64_t>(to) << 32);
}

SparseRelationTable::SparseRelationTable(const reflection::Type& relationType)
    : mRelations{relationType}
{
    mConstructibleTrait = &relationType.get<reflection::ConstructibleTrait>();
}

std::size_t SparseRelationTable::size() const
{
    return mRelations.size();
}

bool SparseRelationTable::insert(uint32_t from, uint32_t to, void* value)
{
    // Single integer which contains both indices.
    auto pair = pairId(from, to);
    if (auto it = mPairRows.find(pair); it != mPairRows.end())
    {
        // Row already exists, simply overwrite it.
        mConstructibleTrait->destruct(mRelations.at(it->second));
        mConstructibleTrait->moveConstruct(mRelations.at(it->second), value);
        return true;
    }

    // Push a new row.
    auto index = static_cast<uint32_t>(mRows.size());
    mRows.emplace_back(Row{
        .from = from,
        .to = to,
        .fromLink = Link{.prev = UINT32_MAX, .next = UINT32_MAX},
        .toLink = Link{.prev = UINT32_MAX, .next = UINT32_MAX},
    });

    // Push the actual data.
    mRelations.pushMove(value);

    // Update the indices.
    mPairRows.emplace(pair, index);
    this->appendLink(index);
    return false;
}

bool SparseRelationTable::erase(uint32_t from, uint32_t to)
{
    // Single integer which contains both indices.
    auto it = mPairRows.find(pairId(from, to));
    if (it == mPairRows.end())
    {
        return false;
    }

    // Update the indices.
    auto index = it->second;
    mPairRows.erase(it);
    this->eraseLink(index);

    // If this wasn't the last row, we first swap it with the last one.
    auto last = static_cast<uint32_t>(mRows.size()) - 1;
    if (index != last)
    {
        // Swap the erased row with the last row.
        mRows[index] = mRows[last];
        mPairRows[pairId(mRows[index].from, mRows[index].to)] = index;
        this->updateLink(index); // Update indices.

        // Do the same for the relation's data.
        mConstructibleTrait->destruct(mRelations.at(index)); // Destruct the erased relation.
        mConstructibleTrait->moveConstruct(mRelations.at(index),
                                           mRelations.at(last)); // Move last into the erased slot.
    }

    // Pop last relation.
    mRows.pop_back();
    mRelations.pop();
    return true;
}

std::size_t SparseRelationTable::eraseFrom(uint32_t from)
{
    std::size_t count = 0;

    while (mFromRows.contains(from))
    {
        auto& row = mRows[mFromRows.at(from).first];
        this->erase(row.from, row.to);
        count += 1;
    }

    return count;
}

std::size_t SparseRelationTable::moveFrom(uint32_t from, SparseRelationTable& other, Transformation transformation)
{
    std::size_t count = 0;

    while (mFromRows.contains(from))
    {
        auto rowIndex = mFromRows.at(from).first;
        auto& row = mRows[rowIndex];
        auto from = row.from;
        auto to = row.to;
        if (transformation == Transformation::Swap || (transformation == Transformation::SwapIfGreater && from > to))
        {
            std::swap(from, to);
        }
        other.insert(from, to, this->at(rowIndex));
        this->erase(row.from, row.to);
        count += 1;
    }

    return count;
}

std::size_t SparseRelationTable::eraseTo(uint32_t to)
{
    std::size_t count = 0;

    while (mToRows.contains(to))
    {
        auto& row = mRows[mToRows.at(to).first];
        this->erase(row.from, row.to);
        count += 1;
    }

    return count;
}

std::size_t SparseRelationTable::moveTo(uint32_t to, SparseRelationTable& other, Transformation transformation)
{
    std::size_t count = 0;

    while (mToRows.contains(to))
    {
        auto rowIndex = mToRows.at(to).first;
        auto& row = mRows[rowIndex];
        auto from = row.from;
        auto to = row.to;
        if (transformation == Transformation::Swap || (transformation == Transformation::SwapIfGreater && from > to))
        {
            std::swap(from, to);
        }
        other.insert(from, to, this->at(rowIndex));
        this->erase(row.from, row.to);
        count += 1;
    }

    return count;
}

bool SparseRelationTable::contains(uint32_t from, uint32_t to) const
{
    return mPairRows.contains(pairId(from, to));
}

std::size_t SparseRelationTable::row(uint32_t from, uint32_t to) const
{
    if (auto it = mPairRows.find(pairId(from, to)); it != mPairRows.end())
    {
        return it->second;
    }

    return this->size();
}

void* SparseRelationTable::at(std::size_t row)
{
    return mRelations.at(row);
}

const void* SparseRelationTable::at(std::size_t row) const
{
    return mRelations.at(row);
}

void SparseRelationTable::indices(std::size_t row, uint32_t& from, uint32_t& to) const
{
    from = mRows[row].from;
    to = mRows[row].to;
}

std::size_t SparseRelationTable::firstFrom(uint32_t index) const
{
    if (auto it = mFromRows.find(index); it != mFromRows.end())
    {
        return it->second.first;
    }

    return this->size();
}

std::size_t SparseRelationTable::firstTo(uint32_t index) const
{
    if (auto it = mToRows.find(index); it != mToRows.end())
    {
        return it->second.first;
    }

    return this->size();
}

std::size_t SparseRelationTable::nextFrom(std::size_t row) const
{
    auto next = mRows[row].fromLink.next;
    return next == UINT32_MAX ? this->size() : static_cast<std::size_t>(next);
}

std::size_t SparseRelationTable::nextTo(std::size_t row) const
{
    auto next = mRows[row].toLink.next;
    return next == UINT32_MAX ? this->size() : static_cast<std::size_t>(next);
}

auto SparseRelationTable::begin() const -> Iterator
{
    return Iterator{*this, 0};
}

auto SparseRelationTable::end() const -> Iterator
{
    return Iterator{*this, static_cast<uint32_t>(mRows.size())};
}

auto SparseRelationTable::viewFrom(uint32_t from) const -> View
{
    return View{*this, from, true};
}

auto SparseRelationTable::viewTo(uint32_t to) const -> View
{
    return View{*this, to, false};
}

void SparseRelationTable::appendLink(uint32_t index)
{
    auto& row = mRows[index];

    if (auto it = mFromRows.find(row.from); it != mFromRows.end())
    {
        // Append the row to the end of the existing list.
        row.fromLink.prev = it->second.last;
        mRows[it->second.last].fromLink.next = index;
        it->second.last = index;
    }
    else
    {
        // There weren't any relations with the same from index before, we must initialize the list.
        mFromRows.emplace(row.from, List{.first = index, .last = index});
    }

    if (auto it = mToRows.find(row.to); it != mToRows.end())
    {
        // Append the row to the end of the existing list.
        row.toLink.prev = it->second.last;
        mRows[it->second.last].toLink.next = index;
        it->second.last = index;
    }
    else
    {
        // There weren't any relations with the same from index before, we must initialize the list.
        mToRows.emplace(row.to, List{.first = index, .last = index});
    }
}

void SparseRelationTable::eraseLink(uint32_t index)
{
    auto& row = mRows[index];

    // Handle 'from' links

    if (row.fromLink.prev == UINT32_MAX)
    {
        // If we're the first, update the first index to be our next.
        mFromRows[row.from].first = row.fromLink.next;
    }
    else
    {
        // Set the next index of the previous to our next.
        mRows[row.fromLink.prev].fromLink.next = row.fromLink.next;
    }

    if (row.fromLink.next == UINT32_MAX)
    {
        // If we're the last, update the last index to be our previous.
        mFromRows[row.from].last = row.fromLink.prev;
    }
    else
    {
        // Set the previous index of the next to our previous.
        mRows[row.fromLink.next].fromLink.prev = row.fromLink.prev;
    }

    if (mFromRows[row.from].first == UINT32_MAX)
    {
        // There aren't any more relations with this from index.
        mFromRows.erase(row.from);
    }

    // Handle 'to' links

    if (row.toLink.prev == UINT32_MAX)
    {
        // If we're the first, update the first index to be our next.
        mToRows[row.to].first = row.toLink.next;
    }
    else
    {
        // Set the next index of the previous to our next.
        mRows[row.toLink.prev].toLink.next = row.toLink.next;
    }

    if (row.toLink.next == UINT32_MAX)
    {
        // If we're the last, update the last index to be our previous.
        mToRows[row.to].last = row.toLink.prev;
    }
    else
    {
        // Set the previous index of the next to our previous.
        mRows[row.toLink.next].toLink.prev = row.toLink.prev;
    }

    if (mToRows[row.to].first == UINT32_MAX)
    {
        // There aren't any more relations with this from index.
        mToRows.erase(row.to);
    }
}

void SparseRelationTable::updateLink(uint32_t index)
{
    auto& row = mRows[index];

    // Handle 'from' links

    if (row.fromLink.prev == UINT32_MAX)
    {
        // If we're the first, we must update the list.
        mFromRows[row.from].first = index;
    }
    else
    {
        // We must update our previous link.
        mRows[row.fromLink.prev].fromLink.next = index;
    }

    if (row.fromLink.next == UINT32_MAX)
    {
        // If we're the last, we must update the list.
        mFromRows[row.from].last = index;
    }
    else
    {
        // We must update our next link.
        mRows[row.fromLink.next].fromLink.prev = index;
    }

    // Handle 'to' links

    if (row.toLink.prev == UINT32_MAX)
    {
        // If we're the first, we must update the list.
        mToRows[row.to].first = index;
    }
    else
    {
        // We must update our previous link.
        mRows[row.toLink.prev].toLink.next = index;
    }

    if (row.toLink.next == UINT32_MAX)
    {
        // If we're the last, we must update the list.
        mToRows[row.to].last = index;
    }
    else
    {
        // We must update our next link.
        mRows[row.toLink.next].toLink.prev = index;
    }
}

SparseRelationTable::Iterator::Iterator(const SparseRelationTable& table, uint32_t row)
    : mTable{table}
    , mRow{row}
{
}

bool SparseRelationTable::Iterator::operator==(const Iterator& other) const
{
    return &mTable == &other.mTable && mRow == other.mRow;
}

auto SparseRelationTable::Iterator::operator*() const -> const Output&
{
    CUBOS_ASSERT(static_cast<std::size_t>(mRow) < mTable.mRows.size(), "Iterator out of bounds");
    mOutput.from = mTable.mRows[mRow].from;
    mOutput.to = mTable.mRows[mRow].to;
    mOutput.value = reinterpret_cast<uintptr_t>(mTable.mRelations.at(mRow));
    return mOutput;
}

auto SparseRelationTable::Iterator::operator->() const -> const Output*
{
    return &this->operator*();
}

auto SparseRelationTable::Iterator::operator++() -> Iterator&
{
    CUBOS_ASSERT(static_cast<std::size_t>(mRow) < mTable.mRows.size(), "Iterator out of bounds");
    ++mRow;
    return *this;
}

SparseRelationTable::View::View(const SparseRelationTable& table, uint32_t index, bool isFrom)
    : mTable{table}
    , mIndex{index}
    , mIsFrom{isFrom}
{
}

auto SparseRelationTable::View::begin() const -> Iterator
{
    auto row = UINT32_MAX;

    if (mIsFrom)
    {
        if (auto it = mTable.mFromRows.find(mIndex); it != mTable.mFromRows.end())
        {
            row = it->second.first;
        }
    }
    else
    {
        if (auto it = mTable.mToRows.find(mIndex); it != mTable.mToRows.end())
        {
            row = it->second.first;
        }
    }

    return Iterator{mTable, row, mIsFrom};
}

auto SparseRelationTable::View::end() const -> Iterator
{
    return Iterator{mTable, UINT32_MAX, mIsFrom};
}

SparseRelationTable::View::Iterator::Iterator(const SparseRelationTable& table, uint32_t row, bool isFrom)
    : mTable{table}
    , mRow{row}
    , mIsFrom{isFrom}
{
}

bool SparseRelationTable::View::Iterator::operator==(const Iterator& other) const
{
    return &mTable == &other.mTable && mRow == other.mRow && mIsFrom == other.mIsFrom;
}

auto SparseRelationTable::View::Iterator::operator*() const -> const Output&
{
    CUBOS_ASSERT(static_cast<std::size_t>(mRow) < mTable.mRows.size(), "Iterator out of bounds");
    mOutput.from = mTable.mRows[mRow].from;
    mOutput.to = mTable.mRows[mRow].to;
    mOutput.value = reinterpret_cast<uintptr_t>(mTable.mRelations.at(mRow));
    return mOutput;
}

auto SparseRelationTable::View::Iterator::operator->() const -> const Output*
{
    return &this->operator*();
}

auto SparseRelationTable::View::Iterator::operator++() -> Iterator&
{
    CUBOS_ASSERT(static_cast<std::size_t>(mRow) < mTable.mRows.size(), "Iterator out of bounds");
    mRow = mIsFrom ? mTable.mRows[mRow].fromLink.next : mTable.mRows[mRow].toLink.next;
    return *this;
}
