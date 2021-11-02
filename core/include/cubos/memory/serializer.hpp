#ifndef CUBOS_MEMORY_SERIALIZER_HPP
#define CUBOS_MEMORY_SERIALIZER_HPP

#include <cubos/memory/stream.hpp>

#include <string>

namespace cubos::memory
{
    /// Abstract class for serializing data.
    class Serializer
    {
    public:
        Serializer() = default;
        virtual ~Serializer() = default;

        /// Serializes a signed 8 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(int8_t value, const char* name = nullptr) = 0;
        
        /// Serializes an signed 16 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(int16_t value, const char* name = nullptr) = 0;

        /// Serializes an signed 32 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(int32_t value, const char* name = nullptr) = 0;

        /// Serializes an signed 64 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(int64_t value, const char* name = nullptr) = 0;

        /// Serializes an unsigned 8 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(uint8_t value, const char* name = nullptr) = 0;

        /// Serializes an unsigned 16 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(uint16_t value, const char* name = nullptr) = 0;

        /// Serializes an unsigned 32 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(uint32_t value, const char* name = nullptr) = 0;

        /// Serializes an unsigned 64 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(uint64_t value, const char* name = nullptr) = 0;

        /// Serializes a float.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(float value, const char* name = nullptr) = 0;

        /// Serializes a double.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(double value, const char* name = nullptr) = 0;

        /// Serializes a string.
        /// @param str The string to serialize.
        /// @param name The name of the value (optional).
        virtual void write(const char* str, const char* name = nullptr) = 0;

    protected:
        /// Indicates that a object is currently being serialized.
        /// @param name The name of the object (optional).
        virtual void beginObject(const char* name = nullptr) = 0;

        /// Indicates that a object is no longer being serialized.
        virtual void endObject() = 0;

        /// Indicates that a array is currently being serialized.
        /// @param length The length of the array.
        /// @param name The name of the array (optional).
        virtual void beginArray(size_t length, const char* name = nullptr) = 0;

        /// Indicates that a array is no longer being serialized.
        virtual void endArray() = 0;

        /// Indicates that a dictionary is currently being serialized.
        /// @param length The length of the dictionary.
        /// @param name The name of the dictionary (optional).
        virtual void beginDictionary(size_t length, const char* name = nullptr) = 0;

        /// Indicates that a dictionary is no longer being serialized.
        virtual void endDictionary() = 0;

    private:
        Serializer(const Serializer&) = delete;
        Serializer& operator=(const Serializer&) = delete;
    };
} // namespace cubos::memory

#endif // CUBOS_MEMORY_SERIALIZER_HPP
