#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <cubos/core/data/old/context.hpp>

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

        Deserializer(const Deserializer&) = delete;
        Deserializer& operator=(const Deserializer&) = delete;

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
        /// @return The length of the array.
        virtual std::size_t beginArray() = 0;

        /// Indicates that an array is no longer being deserialized.
        /// The fail bit is set on failure.
        virtual void endArray() = 0;

        /// Indicates that a dictionary is being deserialized.
        /// The fail bit is set on failure.
        /// @return The length of the dictionary (always 0 on failure).
        virtual std::size_t beginDictionary() = 0;

        /// Indicates that a dictionary is no longer being deserialized.
        /// The fail bit is set on failure.
        virtual void endDictionary() = 0;

        /// Checks if the deserializer has failed.
        bool failed() const;

        /// Sets the fail bit.
        void fail();

        /// @return The context of the serializer.
        inline Context& context()
        {
            return mContext;
        }

    protected:
        bool mFailBit; ///< Indicates if the deserializer has failed.

    private:
        Context mContext; ///< The context of the serializer.
    };

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
    /// @param des The deserializer.
    /// @param obj The value to deserialize.
    void deserialize(Deserializer& des, std::vector<bool>::reference obj);

    /// Overload for deserializing std::vector.
    /// @tparam T The type of the vector.
    /// @param des The deserializer.
    /// @param obj The vector to deserialize.
    template <typename T>
    inline void deserialize(Deserializer& des, std::vector<T>& obj)
    {
        std::size_t length = des.beginArray();
        obj.resize(length);
        for (std::size_t i = 0; i < length; ++i)
        {
            deserialize(des, obj[i]);
        }
        des.endArray();
    }

    /// Overload for deserializing std::unordered_map.
    /// @tparam K The key type of the map.
    /// @tparam V The value type of the map.
    /// @param des The deserializer.
    /// @param obj The map to deserialize.
    template <typename K, typename V>
    inline void deserialize(Deserializer& des, std::unordered_map<K, V>& obj)
    {
        std::size_t length = des.beginDictionary();
        obj.clear();
        obj.reserve(length);
        for (std::size_t i = 0; i < length; ++i)
        {
            K key;
            V value;
            des.read(key);
            des.read(value);
            obj.emplace(std::move(key), std::move(value));
        }
        des.endDictionary();
    }

    /// Overload for serializing std::pair.
    /// @tparam T The type of the first value.
    /// @tparam U The type of the second value.
    /// @param des The deserializer.
    /// @param obj The pair to deserialize.
    template <typename T, typename U>
    inline void deserialize(Deserializer& des, std::pair<T, U>& obj)
    {
        des.beginObject();
        des.read(obj.first);
        des.read(obj.second);
        des.endObject();
    }
} // namespace cubos::core::data
