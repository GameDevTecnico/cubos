#ifndef CUBOS_MEMORY_YAML_SERIALIZER_HPP
#define CUBOS_MEMORY_YAML_SERIALIZER_HPP

#include <cubos/memory/serializer.hpp>

#include <yaml-cpp/emitter.h>
#include <stack>

namespace cubos::memory
{
    /// Implementation of the abstract Serializer class for serializing to YAML.
    class YAMLSerializer : public Serializer
    {
    public:
        /// @param stream The stream to serialize to.
        YAMLSerializer(Stream& stream);
        virtual ~YAMLSerializer() override;

        // Implement interface methods.

        virtual void flush() override;
        virtual void write(int8_t value, const char* name) override;
        virtual void write(int16_t value, const char* name) override;
        virtual void write(int32_t value, const char* name) override;
        virtual void write(int64_t value, const char* name) override;
        virtual void write(uint8_t value, const char* name) override;
        virtual void write(uint16_t value, const char* name) override;
        virtual void write(uint32_t value, const char* name) override;
        virtual void write(uint64_t value, const char* name) override;
        virtual void write(float value, const char* name) override;
        virtual void write(double value, const char* name) override;
        virtual void write(bool value, const char* name) override;
        virtual void write(const char* value, const char* name) override;
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
} // namespace cubos::memory

#endif // CUBOS_MEMORY_YAML_SERIALIZER_HPP
