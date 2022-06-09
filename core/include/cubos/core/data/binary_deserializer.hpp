#ifndef CUBOS_CORE_DATA_BINARY_DESERIALIZER_HPP
#define CUBOS_CORE_DATA_BINARY_DESERIALIZER_HPP

#include <cubos/core/data/deserializer.hpp>

#include <yaml-cpp/yaml.h>
#include <stack>

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

        virtual void readI8(int8_t& value) override;
        virtual void readI16(int16_t& value) override;
        virtual void readI32(int32_t& value) override;
        virtual void readI64(int64_t& value) override;
        virtual void readU8(uint8_t& value) override;
        virtual void readU16(uint16_t& value) override;
        virtual void readU32(uint32_t& value) override;
        virtual void readU64(uint64_t& value) override;
        virtual void readF32(float& value) override;
        virtual void readF64(double& value) override;
        virtual void readBool(bool& value) override;
        virtual void readString(std::string& value) override;
        virtual void beginObject() override;
        virtual void endObject() override;
        virtual size_t beginArray() override;
        virtual void endArray() override;
        virtual size_t beginDictionary() override;
        virtual void endDictionary() override;

    private:
        memory::Stream& stream; ///< The stream to serialize from.
        bool readLittleEndian; ///< Whether to write in little endian or big endian format.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_BINARY_DESERIALIZER_HPP
