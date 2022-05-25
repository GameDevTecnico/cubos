#ifndef CUBOS_CORE_DATA_BINARY_SERIALIZER_HPP
#define CUBOS_CORE_DATA_BINARY_SERIALIZER_HPP

#include <cubos/core/data/serializer.hpp>

#include <stack>

namespace cubos::core::data
{
    /// Implementation of the abstract Serializer class for serializing to binary data.
    /// This class allows data to be serialized in both little and big endian formats.
    class BinarySerializer : public Serializer
    {
    public:
        /// @param stream The stream to serialize to.
        /// @param writeLittleEndian If true, the data will be written in little endian format (big endian otherwise).
        BinarySerializer(memory::Stream& stream, bool writeLittleEndian = true);
        virtual ~BinarySerializer() override = default;

        // Implement interface methods.

        virtual void flush() override;
        virtual void writeI8(int8_t value, const char* name) override;
        virtual void writeI16(int16_t value, const char* name) override;
        virtual void writeI32(int32_t value, const char* name) override;
        virtual void writeI64(int64_t value, const char* name) override;
        virtual void writeU8(uint8_t value, const char* name) override;
        virtual void writeU16(uint16_t value, const char* name) override;
        virtual void writeU32(uint32_t value, const char* name) override;
        virtual void writeU64(uint64_t value, const char* name) override;
        virtual void writeF32(float value, const char* name) override;
        virtual void writeF64(double value, const char* name) override;
        virtual void writeBool(bool value, const char* name) override;
        virtual void writeString(const char* value, const char* name) override;
        virtual void beginObject(const char* name) override;
        virtual void endObject() override;
        virtual void beginArray(size_t length, const char* name) override;
        virtual void endArray() override;
        virtual void beginDictionary(size_t length, const char* name) override;
        virtual void endDictionary() override;

    private:
        bool writeLittleEndian; ///< Whether to write in little endian or big endian format.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_BINARY_SERIALIZER_HPP
