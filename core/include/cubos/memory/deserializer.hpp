#ifndef CUBOS_MEMORY_DESERIALIZER_HPP
#define CUBOS_MEMORY_DESERIALIZER_HPP

#include <cubos/memory/stream.hpp>

#include <string>

namespace cubos::memory
{
    /// Abstract class for deserializing data.
    class Deserializer
    {
    public:
        /// @param stream The stream to deserialize from.
        Deserializer(Stream& stream);
        virtual ~Deserializer() = default;

        /// Deserializes a signed 8 bit integer.
        /// @param value The value to deserialize.
        virtual void read(int8_t& value) = 0;

        /// Deserializes a signed 16 bit integer.
        /// @param value The value to deserialize.
        virtual void read(int16_t& value) = 0;

        /// Deserializes a signed 32 bit integer.
        /// @param value The value to deserialize.
        virtual void read(int32_t& value) = 0;

        /// Deserializes a signed 64 bit integer.
        /// @param value The value to deserialize.
        virtual void read(int64_t& value) = 0;

        /// Deserializes an unsigned 8 bit integer.
        /// @param value The value to deserialize.
        virtual void read(uint8_t& value) = 0;

        /// Deserializes an unsigned 16 bit integer.
        /// @param value The value to deserialize.
        virtual void read(uint16_t& value) = 0;

        /// Deserializes an unsigned 32 bit integer.
        /// @param value The value to deserialize.
        virtual void read(uint32_t& value) = 0;

        /// Deserializes an unsigned 64 bit integer.
        /// @param value The value to deserialize.
        virtual void read(uint64_t& value) = 0;

        /// Deserializes a float.
        /// @param value The value to deserialize.
        virtual void read(float& value) = 0;

        /// Deserializes a double.
        /// @param value The value to deserialize.
        virtual void read(double& value) = 0;

        /// Deserializes a string.
        /// @param value The value to deserialize.
        virtual void read(std::string& value) = 0;

        /// Deserializes a string. A null character is inserted at the end of the string.
        /// @param str The buffer to deserialize the string into.
        /// @param size The size of the buffer.
        virtual void read(char* str, size_t size) = 0;

    protected:
        /// Indicates that a object is currently being deserialized.
        virtual void beginObject() = 0;

        /// Indicates that a object is no longer being deserialized.
        virtual void endObject() = 0;
        
        /// Indicates that an array is currently being deserialized.
        /// Returns the length of the array.
        virtual size_t beginArray() = 0;

        /// Indicates that an array is no longer being deserialized.
        virtual void endArray() = 0;

        /// Indicates that a dictionary is being deserialized.
        /// Returns the length of the dictionary.
        virtual size_t beginDictionary() = 0;

        /// Indicates that a dictionary is no longer being deserialized.
        virtual void endDictionary() = 0;

        Stream& stream; ///< Stream used by the deserializer.

    private:
        Deserializer(const Deserializer&) = delete;
        Deserializer& operator=(const Deserializer&) = delete;
    };
} // namespace cubos::memory

#endif // CUBOS_MEMORY_DESERIALIZER_HPP
