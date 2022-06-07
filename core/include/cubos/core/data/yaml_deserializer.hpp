#ifndef CUBOS_CORE_DATA_YAML_DESERIALIZER_HPP
#define CUBOS_CORE_DATA_YAML_DESERIALIZER_HPP

#include <cubos/core/data/deserializer.hpp>

#include <yaml-cpp/yaml.h>
#include <stack>

namespace cubos::core::data
{
    /// Implementation of the abstract Deserializer class for deserializng from YAML.
    class YAMLDeserializer : public Deserializer
    {
    public:
        /// @param stream The stream to deserialize from.
        YAMLDeserializer(memory::Stream& stream);

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

        /// The current frame of serialization.
        struct Frame
        {
            Mode mode;                 ///< The current mode of deserialization.
            YAML::const_iterator iter; ///< The current node.
            bool key;                  ///< Whether the current node is a key.
        };

        memory::Stream& stream;  ///< The stream to deserialize from.
        std::stack<Frame> frame; ///< The current frame of the deserializer.
        YAML::Node document;     ///< The YAML document being deserialized.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_YAML_DESERIALIZER_HPP
