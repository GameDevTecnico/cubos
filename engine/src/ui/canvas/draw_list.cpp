#include <cstring>
#include <utility>

#include <cubos/core/log.hpp>

#include <cubos/engine/ui/canvas/draw_list.hpp>

cubos::engine::UIDrawList::Type::Type()
{
    for (auto& i : texBindingPoint)
    {
        i = nullptr;
    }
}

cubos::engine::UIDrawList::Entry::Entry(const Type& commandType)
    : type(commandType)
{
}

cubos::engine::UIDrawList::Entry::Entry(const Type& commandType, Command drawCommand, size_t dataOffset)
    : type(commandType)
    , command(std::move(drawCommand))
    , offset(dataOffset)
{
}

cubos::engine::UIDrawList::EntryBuilder::EntryBuilder(Entry& entry)
    : mEntry(entry)
{
}

cubos::engine::UIDrawList::EntryBuilder cubos::engine::UIDrawList::EntryBuilder::withTexture(
    int texIndex, cubos::core::gl::Texture2D texture, cubos::core::gl::Sampler sampler)
{
    if (texIndex >= Type::MaxTextures)
    {
        CUBOS_WARN("Invalid texture index, texture will be dropped");
        return *this;
    }
    mEntry.command.samplers[texIndex] = std::move(sampler);
    mEntry.command.textures[texIndex] = std::move(texture);
    return *this;
}

cubos::engine::UIDrawList::UIDrawList()
{
    mData = operator new(sizeof(char) * mDataCapacity);
}

cubos::engine::UIDrawList::UIDrawList(UIDrawList&& other) noexcept
    : mEntries(std::move(other.mEntries))
    , mData(other.mData)
    , mDataCapacity(other.mDataCapacity)
    , mDataSize(other.mDataSize)
{
    other.mData = nullptr;
    other.mDataSize = 0;
    other.mDataCapacity = 0;
}

cubos::engine::UIDrawList::UIDrawList(const UIDrawList& other) noexcept
    : mEntries(other.mEntries)
    , mDataCapacity(other.mDataSize)
    , mDataSize(other.mDataSize)
{
    mData = operator new(sizeof(char) * other.mDataSize);
    std::memcpy(mData, other.mData, sizeof(char) * other.mDataSize);
}

cubos::engine::UIDrawList::~UIDrawList()
{
    operator delete(mData);
}

cubos::engine::UIDrawList::EntryBuilder cubos::engine::UIDrawList::push(const Type& type, const Command& command,
                                                                        const void* data)
{
    auto entry = Entry{type, command, mDataSize};
    entry.offset = mDataSize;
    mEntries.push_back(entry);
    if (mDataSize + entry.type.perElementSize > mDataCapacity)
    {
        mDataCapacity += entry.type.perElementSize;
        mDataCapacity *= 2;
        void* temp = operator new(sizeof(char) * mDataCapacity);
        std::memcpy(temp, mData, mDataSize);
        operator delete(mData);
        mData = temp;
    }
    std::memcpy(static_cast<char*>(mData) + mDataSize, data, entry.type.perElementSize);
    mDataSize += entry.type.perElementSize;
    return {mEntries[mEntries.size() - 1]};
}

cubos::engine::UIDrawList::EntryBuilder cubos::engine::UIDrawList::push(const Type& type,
                                                                        core::gl::VertexArray vertexArray,
                                                                        size_t vertexOffset, size_t vertexCount,
                                                                        const void* data)
{
    auto command =
        Command{.vertexArray = std::move(vertexArray), .vertexOffset = vertexOffset, .vertexCount = vertexCount};
    return push(type, command, data);
}

void cubos::engine::UIDrawList::clear()
{
    mDataSize = 0;
    mEntries.clear();
}

std::size_t cubos::engine::UIDrawList::size() const
{
    return mEntries.size();
}

const cubos::engine::UIDrawList::Entry& cubos::engine::UIDrawList::entry(std::size_t i) const
{
    return mEntries[i];
}

const void* cubos::engine::UIDrawList::data(std::size_t i) const
{
    return static_cast<void*>(static_cast<char*>(mData) + mEntries[i].offset);
}