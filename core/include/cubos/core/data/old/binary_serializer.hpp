#pragma once

#include <cubos/core/data/old/serializer.hpp>
#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data::old
{
    /// Implementation of the abstract Serializer class for serializing to binary data.
    /// This class allows data to be serialized in both little and big endian formats.
    class BinarySerializer : public Serializer
    {
    public:
        /// @param stream The stream to serialize to.
        /// @param writeLittleEndian If true, the data will be written in little endian format (big endian otherwise).
        BinarySerializer(memory::Stream& stream, bool writeLittleEndian = true);
        ~BinarySerializer() override = default;

        // Implement interface methods.

        void writeI8(int8_t value, const char* name) override;
        void writeI16(int16_t value, const char* name) override;
        void writeI32(int32_t value, const char* name) override;
        void writeI64(int64_t value, const char* name) override;
        void writeU8(uint8_t value, const char* name) override;
        void writeU16(uint16_t value, const char* name) override;
        void writeU32(uint32_t value, const char* name) override;
        void writeU64(uint64_t value, const char* name) override;
        void writeF32(float value, const char* name) override;
        void writeF64(double value, const char* name) override;
        void writeBool(bool value, const char* name) override;
        void writeString(const char* value, const char* name) override;
        void beginObject(const char* name) override;
        void endObject() override;
        void beginArray(std::size_t length, const char* name) override;
        void endArray() override;
        void beginDictionary(std::size_t length, const char* name) override;
        void endDictionary() override;

    private:
        memory::Stream& mStream; ///< The stream to serialize to.
        bool mWriteLittleEndian; ///< Whether to write in little endian or big endian format.
    };
} // namespace cubos::core::data::old
