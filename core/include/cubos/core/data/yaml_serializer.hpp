#ifndef CUBOS_CORE_DATA_YAML_SERIALIZER_HPP
#define CUBOS_CORE_DATA_YAML_SERIALIZER_HPP

#include <cubos/core/data/serializer.hpp>

#include <yaml-cpp/emitter.h>
#include <stack>

namespace cubos::core::data
{
    /// Implementation of the abstract Serializer class for serializing to YAML.
    class YAMLSerializer : public Serializer
    {
    public:
        /// @param stream The stream to serialize to.
        YAMLSerializer(memory::Stream& stream);
        virtual ~YAMLSerializer() override;

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
        /// The possible states of serialization.
        enum class Mode
        {
            Object,
            Array,
            Dictionary
        };

        std::stack<Mode> mode; ///< The current mode of the serializer.
        bool key;              ///< Whether the next write is a key or a value (if we are in a dictionary).
        YAML::Emitter emitter; ///< The YAML emitter.
        size_t head;           ///< The number of characters already flushed.
    };
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_YAML_SERIALIZER_HPP
