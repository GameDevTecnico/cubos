#ifndef CUBOS_CORE_DATA_SERIALIZER_HPP
#define CUBOS_CORE_DATA_SERIALIZER_HPP

#include <cubos/core/data/context.hpp>

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace cubos::core::data
{
    class Serializer;

    /// Serializes the given object using the given serializer.
    /// Serializable objects must implement a specialization (or overload) of this function.
    /// An implementation for types with a `serialize` method is provided.
    ///
    /// @tparam T The type of the object to serialize.
    /// @param ser The serializer to use.
    /// @param obj The object to serialize.
    /// @param name The name of the object.
    template <typename T>
    void serialize(Serializer& ser, const T& obj, const char* name);

    /// Abstract class for serializing data in a format-agnostic way.
    /// Each serializer implementation is responsible for implementing its own primitive
    /// serialization methods: `writeI8`, `writeString`, etc.
    ///
    /// @details More complex types can be serialized by implementing a specialization of the
    /// `cubos::core::data::serialize` function:
    ///
    ///     struct MyType
    ///     {
    ///         int32_t a;
    ///     };
    ///
    ///     // In the corresponding .cpp file.
    ///     void cubos::core::data::serialize<MyType>(Serializer& s, const MyType& obj, const char* name)
    ///     {
    ///         s.write(obj.a, "a");
    ///     }
    ///
    ///     // Alternatively, the type can define a `serialize` method.
    ///
    ///     class MyType
    ///     {
    ///     public:
    ///         void serialize(Serializer& s, const char* name);
    ///
    ///     private:
    ///         int32_t a;
    ///     };
    class Serializer
    {
    public:
        Serializer();
        virtual ~Serializer() = default;

        /// Flushes the serializer, guaranteeing that all data has already been processed.
        /// This is automatically called when the serializer is destroyed.
        virtual void flush();

        /// Serializes a signed 8 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeI8(int8_t value, const char* name) = 0;

        /// Serializes an signed 16 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeI16(int16_t value, const char* name) = 0;

        /// Serializes an signed 32 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeI32(int32_t value, const char* name) = 0;

        /// Serializes an signed 64 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeI64(int64_t value, const char* name) = 0;

        /// Serializes an unsigned 8 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeU8(uint8_t value, const char* name) = 0;

        /// Serializes an unsigned 16 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeU16(uint16_t value, const char* name) = 0;

        /// Serializes an unsigned 32 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeU32(uint32_t value, const char* name) = 0;

        /// Serializes an unsigned 64 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeU64(uint64_t value, const char* name) = 0;

        /// Serializes a float.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeF32(float value, const char* name) = 0;

        /// Serializes a double.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeF64(double value, const char* name) = 0;

        /// Serializes a boolean.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void writeBool(bool value, const char* name) = 0;

        /// Serializes a string.
        /// @param str The string to serialize.
        /// @param name The name of the value (optional).
        virtual void writeString(const char* str, const char* name) = 0;

        /// Serializes an object.
        /// The `cubos::core::data::serialize` function must be implemented for the given type.
        /// @tparam T The type of the object.
        /// @param obj The object to serialize.
        /// @param name The name of the object (optional).
        template <typename T>
        inline void write(const T& obj, const char* name)
        {
            serialize(*this, obj, name);
        }

        /// Indicates that a object is currently being serialized.
        /// @param name The name of the object (optional).
        virtual void beginObject(const char* name) = 0;

        /// Indicates that a object is no longer being serialized.
        virtual void endObject() = 0;

        /// Indicates that a array is currently being serialized.
        /// @param length The length of the array.
        /// @param name The name of the array (optional).
        virtual void beginArray(size_t length, const char* name) = 0;

        /// Indicates that a array is no longer being serialized.
        virtual void endArray() = 0;

        /// Indicates that a dictionary is currently being serialized.
        /// @param length The length of the dictionary.
        /// @param name The name of the dictionary (optional).
        virtual void beginDictionary(size_t length, const char* name) = 0;

        /// Indicates that a dictionary is no longer being serialized.
        virtual void endDictionary() = 0;

        /// Checks if the serializer has failed.
        bool failed() const;

        /// @returns The context of the serializer.
        inline Context& context()
        {
            return this->m_context;
        }

    protected:
        bool failBit; ///< Indicates if the serializer failed.

    private:
        Serializer(const Serializer&) = delete;
        Serializer& operator=(const Serializer&) = delete;

        Context m_context; ///< The context of the serializer.
    };

    /// Concept for serializable objects which define a serialize method.
    template <typename T>
    concept HasSerializeMethod = requires(Serializer& ser, const T& obj, const char* name)
    {
        // clang-format off
        { obj.serialize(ser, name) } -> std::same_as<void>;
        // clang-format on
    };

    /// Overload the serialize function for types which define a serialize method.
    template <typename T>
    requires HasSerializeMethod<T>
    inline void serialize(Serializer& ser, const T& obj, const char* name)
    {
        obj.serialize(ser, name);
    }

    /// Overload for serializing std::vector.
    /// @tparam T The type of the vector.
    /// @param ser The serializer.
    /// @param vec The vector to serialize.
    /// @param name The name of the vector (optional).
    template <typename T>
    inline void serialize(Serializer& ser, const std::vector<T>& vec, const char* name)
    {
        ser.beginArray(vec.size(), name);
        for (const auto& obj : vec)
            ser.write(obj, nullptr);
        ser.endArray();
    }

    /// Overload for serializing std::unordered_map.
    /// @tparam K The type of the key.
    /// @tparam V The type of the value.
    /// @param ser The serializer.
    /// @param map The map to serialize.
    /// @param name The name of the map (optional).
    template <typename K, typename V>
    inline void serialize(Serializer& ser, const std::unordered_map<K, V>& map, const char* name)
    {
        ser.beginDictionary(map.size(), name);
        for (const auto& pair : map)
        {
            ser.write(pair.first, nullptr);
            ser.write(pair.second, nullptr);
        }
        ser.endDictionary();
    }
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_SERIALIZER_HPP
