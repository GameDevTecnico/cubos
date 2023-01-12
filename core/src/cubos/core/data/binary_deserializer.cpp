#include <cubos/core/data/binary_deserializer.hpp>

#include <cubos/core/memory/endianness.hpp>

using namespace cubos::core;
using namespace cubos::core::data;

template <typename T>
static inline T fromEndianness(T val, bool fromLittleEndian)
{
    if (fromLittleEndian)
        return memory::fromLittleEndian(val);
    else
        return memory::fromBigEndian(val);
}

BinaryDeserializer::BinaryDeserializer(memory::Stream& stream, bool readLittleEndian) : stream(stream)
{
    this->readLittleEndian = readLittleEndian;
}

void BinaryDeserializer::readI8(int8_t& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
}

void BinaryDeserializer::readI16(int16_t& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, this->readLittleEndian);
}

void BinaryDeserializer::readI32(int32_t& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, this->readLittleEndian);
}

void BinaryDeserializer::readI64(int64_t& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, this->readLittleEndian);
}

void BinaryDeserializer::readU8(uint8_t& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, this->readLittleEndian);
}

void BinaryDeserializer::readU16(uint16_t& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, this->readLittleEndian);
}

void BinaryDeserializer::readU32(uint32_t& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, this->readLittleEndian);
}

void BinaryDeserializer::readU64(uint64_t& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, this->readLittleEndian);
}

void BinaryDeserializer::readF32(float& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, this->readLittleEndian);
}

void BinaryDeserializer::readF64(double& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
    value = fromEndianness(value, this->readLittleEndian);
}

void BinaryDeserializer::readBool(bool& value)
{
    this->failBit |= stream.read(&value, sizeof(value)) != sizeof(value);
}

void BinaryDeserializer::readString(std::string& value)
{
    this->stream.readUntil(value, nullptr);
}

void BinaryDeserializer::beginObject()
{
    // Do nothing.
}

void BinaryDeserializer::endObject()
{
    // Do nothing.
}

size_t BinaryDeserializer::beginArray()
{
    uint64_t size;
    this->readU64(size);
    return size;
}

void BinaryDeserializer::endArray()
{
    // Do nothing.
}

size_t BinaryDeserializer::beginDictionary()
{
    uint64_t size;
    this->readU64(size);
    return size;
}

void BinaryDeserializer::endDictionary()
{
    // Do nothing.
}
