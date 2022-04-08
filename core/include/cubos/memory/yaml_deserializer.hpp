#ifndef CUBOS_CORE_MEMORY_YAML_DESERIALIZER_HPP
#define CUBOS_CORE_MEMORY_YAML_DESERIALIZER_HPP

#include <cubos/memory/deserializer.hpp>

#include <yaml-cpp/yaml.h>
#include <stack>

namespace cubos::core::memory
{
    /// Implementation of the abstract Deserializer class for deserializng from YAML.
    class YAMLDeserializer : public Deserializer
    {
    public:
        /// @param stream The stream to deserialize from.
        YAMLDeserializer(Stream& stream);

        // Implement interface methods.

        virtual void read(int8_t& value) override;
        virtual void read(int16_t& value) override;
        virtual void read(int32_t& value) override;
        virtual void read(int64_t& value) override;
        virtual void read(uint8_t& value) override;
        virtual void read(uint16_t& value) override;
        virtual void read(uint32_t& value) override;
        virtual void read(uint64_t& value) override;
        virtual void read(float& value) override;
        virtual void read(double& value) override;
        virtual void read(bool& value) override;
        virtual void read(std::string& value) override;
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

        std::stack<Frame> frame; ///< The current frame of the deserializer.
        YAML::Node document;     ///< The YAML document being deserialized.
    };
} // namespace cubos::core::memory

#endif // CUBOS_CORE_MEMORY_YAML_DESERIALIZER_HPP
