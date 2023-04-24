#include <cassert>

#include <cubos/core/data/binary_serializer.hpp>
#include <cubos/core/memory/endianness.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

template <typename T>
static inline T toEndianness(T val, bool toLittleEndian)
{
    if (toLittleEndian)
        return memory::toLittleEndian(val);
    else
        return memory::toBigEndian(val);
}

BinarySerializer::BinarySerializer(memory::Stream& stream, bool writeLittleEndian)
    : stream(stream)
{
    this->writeLittleEndian = writeLittleEndian;
}

void BinarySerializer::writeI8(int8_t value, const char*)
{
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeI16(int16_t value, const char*)
{
    value = toEndianness(value, this->writeLittleEndian);
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeI32(int32_t value, const char*)
{
    value = toEndianness(value, this->writeLittleEndian);
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeI64(int64_t value, const char*)
{
    value = toEndianness(value, this->writeLittleEndian);
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeU8(uint8_t value, const char*)
{
    value = toEndianness(value, this->writeLittleEndian);
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeU16(uint16_t value, const char*)
{
    value = toEndianness(value, this->writeLittleEndian);
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeU32(uint32_t value, const char*)
{
    value = toEndianness(value, this->writeLittleEndian);
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeU64(uint64_t value, const char*)
{
    value = toEndianness(value, this->writeLittleEndian);
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeF32(float value, const char*)
{
    value = toEndianness(value, this->writeLittleEndian);
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeF64(double value, const char*)
{
    value = toEndianness(value, this->writeLittleEndian);
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeBool(bool value, const char*)
{
    this->failBit |= this->stream.write(&value, sizeof(value)) == sizeof(value);
}

void BinarySerializer::writeString(const char* value, const char*)
{
    assert(value != nullptr);
    this->stream.print(value);
    this->stream.put('\0');
}

void BinarySerializer::beginObject(const char*)
{
    // Do nothing.
}

void BinarySerializer::endObject()
{
    // Do nothing.
}

void BinarySerializer::beginArray(size_t length, const char* name)
{
    this->writeU64(static_cast<uint64_t>(length), name);
}

void BinarySerializer::endArray()
{
    // Do nothing.
}

void BinarySerializer::beginDictionary(size_t length, const char* name)
{
    this->writeU64(static_cast<uint64_t>(length), name);
}

void BinarySerializer::endDictionary()
{
    // Do nothing.
}
