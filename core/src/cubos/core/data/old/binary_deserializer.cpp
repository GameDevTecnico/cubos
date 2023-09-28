#include <cubos/core/data/old/binary_deserializer.hpp>
#include <cubos/core/memory/endianness.hpp>

using namespace cubos::core;
using namespace cubos::core::data::old;

template <typename T>
static inline T fromEndianness(T val, bool fromLittleEndian)
{
    if (fromLittleEndian)
    {
        return memory::fromLittleEndian(val);
    }
    return memory::fromBigEndian(val);
}

BinaryDeserializer::BinaryDeserializer(memory::Stream& stream, bool readLittleEndian)
    : mStream(stream)
{
    mReadLittleEndian = readLittleEndian;
}

void BinaryDeserializer::readI8(int8_t& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
}

void BinaryDeserializer::readI16(int16_t& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, mReadLittleEndian);
}

void BinaryDeserializer::readI32(int32_t& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, mReadLittleEndian);
}

void BinaryDeserializer::readI64(int64_t& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, mReadLittleEndian);
}

void BinaryDeserializer::readU8(uint8_t& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, mReadLittleEndian);
}

void BinaryDeserializer::readU16(uint16_t& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, mReadLittleEndian);
}

void BinaryDeserializer::readU32(uint32_t& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, mReadLittleEndian);
}

void BinaryDeserializer::readU64(uint64_t& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, mReadLittleEndian);
}

void BinaryDeserializer::readF32(float& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, mReadLittleEndian);
}

void BinaryDeserializer::readF64(double& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, mReadLittleEndian);
}

void BinaryDeserializer::readBool(bool& value)
{
    mFailBit |= mStream.read(&value, sizeof(value)) != sizeof(value);
}

void BinaryDeserializer::readString(std::string& value)
{
    mStream.readUntil(value, nullptr);
}

void BinaryDeserializer::beginObject()
{
    // Do nothing.
}

void BinaryDeserializer::endObject()
{
    // Do nothing.
}

std::size_t BinaryDeserializer::beginArray()
{
    uint64_t size;
    this->readU64(size);
    return (std::size_t)size;
}

void BinaryDeserializer::endArray()
{
    // Do nothing.
}

std::size_t BinaryDeserializer::beginDictionary()
{
    uint64_t size;
    this->readU64(size);
    return (std::size_t)size;
}

void BinaryDeserializer::endDictionary()
{
    // Do nothing.
}
