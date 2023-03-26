#pragma once

#include <stack>

#include <yaml-cpp/yaml.h>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    /// Implementation of the abstract Deserializer class for deserializng from YAML.
    class YAMLDeserializer : public Deserializer
    {
    public:
        /// @param stream The stream to deserialize from.
        YAMLDeserializer(memory::Stream& stream);

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
        /// Loads a new YAML document from the stream.
        void loadDocument();

        /// Returns the next node to be read.
        YAML::const_iterator get();

        /// The possible states of deserialization.
        enum class Mode
        {
            Object,
            Array,
            Dictionary
        };

        /// The current frame of deserialization.
        struct Frame
        {
            Mode mode;                 ///< The current mode of deserialization.
            YAML::const_iterator iter; ///< The current node.
            bool key;                  ///< Whether the current node is a key.
        };

        memory::Stream& mStream;  ///< The stream to deserialize from.
        std::stack<Frame> mFrame; ///< The current frame of the deserializer.
        YAML::Node mDocument;     ///< The YAML document being deserialized.
    };
} // namespace cubos::core::data
