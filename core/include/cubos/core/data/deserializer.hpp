#ifndef CUBOS_CORE_DATA_DESERIALIZER_HPP
#define CUBOS_CORE_DATA_DESERIALIZER_HPP

#include <cubos/core/data/context.hpp>

#include <vector>
#include <unordered_map>
#include <string>

namespace cubos::core::data
{
    class Deserializer;

    /// Deserializes the given object using the given deserializer.
    /// Deserializable objects must implement a specialization (or overload) of this function.
    /// An implementation for types with a `deserialize` method is provided.
    ///
    /// @tparam T The type of the object to deserialize.
    /// @param des The deserializer to use.
    /// @param obj The object to deserialize.
    template <typename T>
    void deserialize(Deserializer& des, T& obj);

    /// Abstract class for deserializing data in a format-agnostic way.
    /// Each deserializer implementation is responsible for implementing its own primitive
    /// deserialization methods: `readI8`, `readString`, etc.
    ///
    /// @details More complex types can be deserialized by implementing a specialization of the
    /// `cubos::core::data::deserialize` function:
    ///
    ///     struct MyType
    ///     {
    ///         int32_t a;
    ///     };
    ///
    ///     // In the corresponding .cpp file.
    ///     void cubos::core::data::deserialize<MyType>(Deserializer& d, MyType& obj)
    ///     {
    ///         d.read(obj.a, "a");
    ///     }
    ///
    ///     // Alternatively, the type can define a `deserialize` method.
    ///
    ///     class MyType
    ///     {
    ///     public:
    ///         void deserialize(Deserializer& d);
    ///
    ///     private:
    ///         int32_t a;
    ///     };
    class Deserializer
    {
    public:
        Deserializer();
        virtual ~Deserializer() = default;

        /// Deserializes a signed 8 bit integer.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readI8(int8_t& value) = 0;

        /// Deserializes a signed 16 bit integer.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readI16(int16_t& value) = 0;

        /// Deserializes a signed 32 bit integer.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readI32(int32_t& value) = 0;

        /// Deserializes a signed 64 bit integer.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readI64(int64_t& value) = 0;

        /// Deserializes an unsigned 8 bit integer.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readU8(uint8_t& value) = 0;

        /// Deserializes an unsigned 16 bit integer.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readU16(uint16_t& value) = 0;

        /// Deserializes an unsigned 32 bit integer.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readU32(uint32_t& value) = 0;

        /// Deserializes an unsigned 64 bit integer.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readU64(uint64_t& value) = 0;

        /// Deserializes a float.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readF32(float& value) = 0;

        /// Deserializes a double.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readF64(double& value) = 0;

        /// Deserializes a boolean.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readBool(bool& value) = 0;

        /// Deserializes a string.
        /// The fail bit is set if the deserialization fails.
        /// @param value The value to deserialize.
        virtual void readString(std::string& value) = 0;

        /// Deserializes an object.
        /// The `cubos::core::data::deserialize` function must be implemented for the given type.
        /// The fail bit is set if the deserialization fails.
        /// @tparam T The type of the object.
        /// @param obj The object to deserialize.
        template <typename T>
        inline void read(T& obj)
        {
            deserialize(*this, obj);
        }

        /// Indicates that a object is currently being deserialized.
        /// The fail bit is set on failure.
        virtual void beginObject() = 0;

        /// Indicates that a object is no longer being deserialized.
        /// The fail bit is set on failure.
        virtual void endObject() = 0;

        /// Indicates that an array is currently being deserialized.
        /// The fail bit is set on failure.
        /// @returns The length of the array.
        virtual size_t beginArray() = 0;

        /// Indicates that an array is no longer being deserialized.
        /// The fail bit is set on failure.
        virtual void endArray() = 0;

        /// Indicates that a dictionary is being deserialized.
        /// The fail bit is set on failure.
        /// @returns The length of the dictionary (always 0 on failure).
        virtual size_t beginDictionary() = 0;

        /// Indicates that a dictionary is no longer being deserialized.
        /// The fail bit is set on failure.
        virtual void endDictionary() = 0;

        /// Checks if the deserializer has failed.
        bool failed() const;

        /// @returns The context of the serializer.
        inline Context& context()
        {
            return this->m_context;
        }

    protected:
        bool failBit; ///< Indicates if the deserializer has failed.

    private:
        Deserializer(const Deserializer&) = delete;
        Deserializer& operator=(const Deserializer&) = delete;

        Context m_context; ///< The context of the serializer.
    };

    /// Concept for deserializable objects which define a deserialize method.
    template <typename T>
    concept HasDeserializeMethod = requires(Deserializer& des, T& obj)
    {
        // clang-format off
        { obj.deserialize(des) } -> std::same_as<void>;
        // clang-format on
    };

    /// Overload the serialize function for types which define a serialize method.
    template <typename T>
    requires HasDeserializeMethod<T>
    inline void deserialize(Deserializer& des, T& obj)
    {
        obj.deserialize(des);
    }

    /// Overload for deserializing std::vector<bool>::reference.
    /// This is a special case because std::vector<bool> are stored as arrays of bits, and therefore
    /// need special handling.
    void deserialize(Deserializer& des, std::vector<bool>::reference val);

    /// Overload for deserializing std::vector.
    /// @tparam T The type of the vector.
    /// @param des The deserializer.
    /// @param vec The vector to deserialize.
    template <typename T>
    inline void deserialize(Deserializer& des, std::vector<T>& vec)
    {
        size_t length = des.beginArray();
        vec.resize(length);
        for (size_t i = 0; i < length; ++i)
            deserialize(des, vec[i]);
        des.endArray();
    }

    /// Overload for deserializing std::unordered_map.
    /// @tparam K The key type of the map.
    /// @tparam V The value type of the map.
    /// @param des The deserializer.
    /// @param map The map to deserialize.
    template <typename K, typename V>
    inline void deserialize(Deserializer& des, std::unordered_map<K, V>& map)
    {
        size_t length = des.beginDictionary();
        map.clear();
        map.reserve(length);
        for (size_t i = 0; i < length; ++i)
        {
            K key;
            V value;
            des.read(key);
            des.read(value);
            map.emplace(std::move(key), std::move(value));
        }
        des.endDictionary();
    }
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_DESERIALIZER_HPP
