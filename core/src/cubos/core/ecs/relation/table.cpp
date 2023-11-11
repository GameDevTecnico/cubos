#include <cubos/core/ecs/relation/table.hpp>
#include <cubos/core/log.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

using cubos::core::ecs::RelationTable;

/// @brief Creates a single integer which uniquely identifies the relation with the given indices.
/// @param from From index.
/// @param to To index.
/// @return Identifier.
static uint64_t pairId(uint32_t from, uint32_t to)
{
    return static_cast<uint64_t>(from) | (static_cast<uint64_t>(to) << 32);
}

RelationTable::RelationTable(const reflection::Type& relationType)
    : mRelations{relationType}
{
    mConstructibleTrait = &relationType.get<reflection::ConstructibleTrait>();
}

std::size_t RelationTable::size() const
{
    return mRelations.size();
}

bool RelationTable::insert(uint32_t from, uint32_t to, void* value)
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

bool RelationTable::erase(uint32_t from, uint32_t to)
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

std::size_t RelationTable::eraseFrom(uint32_t from)
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

std::size_t RelationTable::eraseTo(uint32_t to)
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

bool RelationTable::contains(uint32_t from, uint32_t to) const
{
    return mPairRows.contains(pairId(from, to));
}

uintptr_t RelationTable::at(uint32_t from, uint32_t to) const
{
    if (auto it = mPairRows.find(pairId(from, to)); it != mPairRows.end())
    {
        return reinterpret_cast<uintptr_t>(mRelations.at(it->second));
    }

    return 0;
}

auto RelationTable::begin() const -> Iterator
{
    return Iterator{*this, 0};
}

auto RelationTable::end() const -> Iterator
{
    return Iterator{*this, static_cast<uint32_t>(mRows.size())};
}

auto RelationTable::viewFrom(uint32_t from) const -> View
{
    return View{*this, from, true};
}

auto RelationTable::viewTo(uint32_t to) const -> View
{
    return View{*this, to, false};
}

void RelationTable::appendLink(uint32_t index)
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

void RelationTable::eraseLink(uint32_t index)
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

void RelationTable::updateLink(uint32_t index)
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

RelationTable::Iterator::Iterator(const RelationTable& table, uint32_t row)
    : mTable{table}
    , mRow{row}
{
}

bool RelationTable::Iterator::operator==(const Iterator& other) const
{
    return &mTable == &other.mTable && mRow == other.mRow;
}

auto RelationTable::Iterator::operator*() const -> const Output&
{
    CUBOS_ASSERT(static_cast<std::size_t>(mRow) < mTable.mRows.size(), "Iterator out of bounds");
    mOutput.from = mTable.mRows[mRow].from;
    mOutput.to = mTable.mRows[mRow].to;
    mOutput.value = reinterpret_cast<uintptr_t>(mTable.mRelations.at(mRow));
    return mOutput;
}

auto RelationTable::Iterator::operator->() const -> const Output*
{
    return &this->operator*();
}

auto RelationTable::Iterator::operator++() -> Iterator&
{
    CUBOS_ASSERT(static_cast<std::size_t>(mRow) < mTable.mRows.size(), "Iterator out of bounds");
    ++mRow;
    return *this;
}

RelationTable::View::View(const RelationTable& table, uint32_t index, bool isFrom)
    : mTable{table}
    , mIndex{index}
    , mIsFrom{isFrom}
{
}

auto RelationTable::View::begin() const -> Iterator
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

auto RelationTable::View::end() const -> Iterator
{
    return Iterator{mTable, UINT32_MAX, mIsFrom};
}

RelationTable::View::Iterator::Iterator(const RelationTable& table, uint32_t row, bool isFrom)
    : mTable{table}
    , mRow{row}
    , mIsFrom{isFrom}
{
}

bool RelationTable::View::Iterator::operator==(const Iterator& other) const
{
    return &mTable == &other.mTable && mRow == other.mRow && mIsFrom == other.mIsFrom;
}

auto RelationTable::View::Iterator::operator*() const -> const Output&
{
    CUBOS_ASSERT(static_cast<std::size_t>(mRow) < mTable.mRows.size(), "Iterator out of bounds");
    mOutput.from = mTable.mRows[mRow].from;
    mOutput.to = mTable.mRows[mRow].to;
    mOutput.value = reinterpret_cast<uintptr_t>(mTable.mRelations.at(mRow));
    return mOutput;
}

auto RelationTable::View::Iterator::operator->() const -> const Output*
{
    return &this->operator*();
}

auto RelationTable::View::Iterator::operator++() -> Iterator&
{
    CUBOS_ASSERT(static_cast<std::size_t>(mRow) < mTable.mRows.size(), "Iterator out of bounds");
    mRow = mIsFrom ? mTable.mRows[mRow].fromLink.next : mTable.mRows[mRow].toLink.next;
    return *this;
}
