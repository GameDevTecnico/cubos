#include <cassert>

#include <cubos/core/data/old/binary_serializer.hpp>
#include <cubos/core/memory/endianness.hpp>

using namespace cubos::core;
using namespace cubos::core::data::old;

template <typename T>
static inline T toEndianness(T val, bool toLittleEndian)
{
    if (toLittleEndian)
    {
        return memory::toLittleEndian(val);
    }
    return memory::toBigEndian(val);
}

BinarySerializer::BinarySerializer(memory::Stream& stream, bool writeLittleEndian)
    : mStream(stream)
{
    mWriteLittleEndian = writeLittleEndian;
}

void BinarySerializer::writeI8(int8_t value, const char* /*name*/)
{
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeI16(int16_t value, const char* /*name*/)
{
    value = toEndianness(value, mWriteLittleEndian);
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeI32(int32_t value, const char* /*name*/)
{
    value = toEndianness(value, mWriteLittleEndian);
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeI64(int64_t value, const char* /*name*/)
{
    value = toEndianness(value, mWriteLittleEndian);
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeU8(uint8_t value, const char* /*name*/)
{
    value = toEndianness(value, mWriteLittleEndian);
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeU16(uint16_t value, const char* /*name*/)
{
    value = toEndianness(value, mWriteLittleEndian);
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeU32(uint32_t value, const char* /*name*/)
{
    value = toEndianness(value, mWriteLittleEndian);
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeU64(uint64_t value, const char* /*name*/)
{
    value = toEndianness(value, mWriteLittleEndian);
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeF32(float value, const char* /*name*/)
{
    value = toEndianness(value, mWriteLittleEndian);
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeF64(double value, const char* /*name*/)
{
    value = toEndianness(value, mWriteLittleEndian);
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeBool(bool value, const char* /*name*/)
{
    mFailBit |= mStream.write(&value, sizeof(value)) != sizeof(value);
}

void BinarySerializer::writeString(const char* value, const char* /*name*/)
{
    assert(value != nullptr);
    mStream.print(value);
    mStream.put('\0');
}

void BinarySerializer::beginObject(const char* /*name*/)
{
    // Do nothing.
}

void BinarySerializer::endObject()
{
    // Do nothing.
}

void BinarySerializer::beginArray(std::size_t length, const char* name)
{
    this->writeU64(static_cast<uint64_t>(length), name);
}

void BinarySerializer::endArray()
{
    // Do nothing.
}

void BinarySerializer::beginDictionary(std::size_t length, const char* name)
{
    this->writeU64(static_cast<uint64_t>(length), name);
}

void BinarySerializer::endDictionary()
{
    // Do nothing.
}
