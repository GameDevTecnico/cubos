#pragma once

#include <yaml-cpp/yaml.h>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    /// Implementation of the abstract Deserializer class for deserializing from raw binary data.
    /// This class allows data to be deserialized from both little and big endian formats.
    class BinaryDeserializer : public Deserializer
    {
    public:
        /// @param stream The stream to deserialize from.
        /// @param readLittleEndian If true, the data will be read in little endian format (big endian otherwise).
        BinaryDeserializer(memory::Stream& stream, bool readLittleEndian = true);

        // Implement interface methods.

        void readI8(int8_t& value) override;
        void readI16(int16_t& value) override;
        void readI32(int32_t& value) override;
        void readI64(int64_t& value) override;
        void readU8(uint8_t& value) override;
        void readU16(uint16_t& value) override;
        void readU32(uint32_t& value) override;
        void readU64(uint64_t& value) override;
        void readF32(float& value) override;
        void readF64(double& value) override;
        void readBool(bool& value) override;
        void readString(std::string& value) override;
        void beginObject() override;
        void endObject() override;
        std::size_t beginArray() override;
        void endArray() override;
        std::size_t beginDictionary() override;
        void endDictionary() override;

    private:
        memory::Stream& mStream; ///< The stream to serialize from.
        bool mReadLittleEndian;  ///< Whether to write in little endian or big endian format.
    };
} // namespace cubos::core::data
