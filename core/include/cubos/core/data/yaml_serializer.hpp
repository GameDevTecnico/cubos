#ifndef CUBOS_CORE_DATA_YAML_SERIALIZER_HPP
#define CUBOS_CORE_DATA_YAML_SERIALIZER_HPP

#include <stack>

#include <yaml-cpp/emitter.h>

#include <cubos/core/data/serializer.hpp>
#include <cubos/core/memory/stream.hpp>

namespace cubos::core::data
{
    /// Implementation of the abstract Serializer class for serializing to YAML.
    class YAMLSerializer : public Serializer
    {
    public:
        /// @param stream The stream to serialize to.
        YAMLSerializer(memory::Stream& stream);
        ~YAMLSerializer() override;

        // Implement interface methods.

        void flush() override;
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
        /// The possible states of serialization.
        enum class Mode
        {
            Object,
            Array,
            Dictionary
        };

        memory::Stream& mStream; ///< The stream to serialize to.
        std::stack<Mode> mode;  ///< The current mode of the serializer.
        bool key;               ///< Whether the next write is a key or a value (if we are in a dictionary).
        YAML::Emitter emitter;  ///< The YAML emitter.
        std::size_t mHead;      ///< The number of characters already flushed.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_YAML_SERIALIZER_HPP
