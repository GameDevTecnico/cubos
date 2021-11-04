#ifndef CUBOS_MEMORY_SERIALIZER_HPP
#define CUBOS_MEMORY_SERIALIZER_HPP

#include <cubos/memory/stream.hpp>
#include <cubos/memory/serialization_map.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <concepts>

namespace cubos::memory
{
    class Serializer;

    /// Concept for serializable objects which are trivial to serialize.
    template <typename T> concept TriviallySerializable = requires(T obj, Serializer& s)
    {
        {
            obj.serialize(s)
        }
        ->std::same_as<void>;
    };

    /// Concept for serializable objects which require a serialization map.
    template <typename T, typename R, typename I>
    concept MapSerializable = requires(T obj, Serializer& s, SerializationMap<R, I>& map)
    {
        {
            obj.serialize(s, map)
        }
        ->std::same_as<void>;
    };

    /// Abstract class for serializing data.
    class Serializer
    {
    public:
        /// @param stream The stream to serialize to.
        Serializer(Stream& stream);
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

        /// Serializes a object.
        /// @tparam T The type of the object.
        /// @param obj The object to serialize.
        /// @param name The name of the object (optional).
        template <typename T> requires TriviallySerializable<T> void write(const T& obj, const char* name = nullptr);

        /// Serializes a object.
        /// @tparam T The type of the object.
        /// @tparam R The reference type.
        /// @tparam I The serialized identifier type.
        /// @param obj The object to serialize.
        /// @param map The serialization map.
        /// @param name The name of the object (optional).
        template <typename T, typename R, typename I>
        requires TriviallySerializable<T> void write(const T& obj, SerializationMap<R, I>& map,
                                                     const char* name = nullptr);

        /// Serializes a object which requires a serialization map to be serialized.
        /// @tparam T The type of the object.
        /// @tparam R The reference type.
        /// @tparam I The serialized identifier type.
        /// @param obj The object to serialize.
        /// @param map The serialization map.
        /// @param name The name of the object (optional).
        template <typename T, typename R, typename I>
        requires MapSerializable<T, R, I> void write(const T& obj, SerializationMap<R, I>& map,
                                                     const char* name = nullptr);

        /// Serializes a object which requires a serialization map to be serialized.
        /// @tparam T The type of the object.
        /// @tparam R The reference type.
        /// @tparam I The serialized identifier type.
        /// @param obj The object to serialize.
        /// @param map The serialization map.
        /// @param name The name of the object (optional).
        template <typename T, typename R, typename I>
        void write(const T& obj, SerializationMap<R, I>& map, const char* name = nullptr);

        /// Serializes a vector.
        /// @tparam T The type of the vector.
        /// @param vec The vector to serialize.
        /// @param name The name of the vector (optional).
        template <typename T> void write(const std::vector<T>& vec, const char* name = nullptr);

        /// Serializes an array.
        /// @tparam T The type of the array.
        /// @param ptr The array to serialize.
        /// @param length The length of the array.
        /// @param name The name of the array (optional).
        template <typename T> void write(const T* ptr, size_t length, const char* name = nullptr);

        /// Serializes a unordered map.
        /// @tparam K The key type.
        /// @tparam V The value type.
        /// @param dic The map to serialize.
        /// @param name The name of the map (optional).
        template <typename K, typename V> void write(const std::unordered_map<K, V>& dic, const char* name = nullptr);

        /// Serializes a std::vector.
        /// @tparam T The type of the vector.
        /// @tparam R The reference type.
        /// @tparam I The serialized identifier type.
        /// @param vec The vector to serialize.
        /// @param map The serialization map.
        /// @param name The name of the vector (optional).
        template <typename T, typename R, typename I>
        void write(const std::vector<T>& vec, SerializationMap<R, I>& map, const char* name = nullptr);

        /// Serializes an array.
        /// @tparam T The type of the array.
        /// @tparam R The reference type.
        /// @tparam I The serialized identifier type.
        /// @param ptr The array to serialize.
        /// @param length The length of the array.
        /// @param map The serialization map.
        /// @param name The name of the array (optional).
        template <typename T, typename R, typename I>
        void write(const T* ptr, size_t length, SerializationMap<R, I>& map, const char* name = nullptr);

        /// Serializes a unordered map.
        /// @tparam K The key type.
        /// @tparam V The value type.
        /// @param dic The map to serialize.
        /// @param name The name of the map (optional).
        template <typename K, typename V, typename R, typename I>
        void write(const std::unordered_map<K, V>& dic, SerializationMap<R, I>& map, const char* name = nullptr);

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

        Stream& stream; ///< Stream used by the serializer.

    private:
        Serializer(const Serializer&) = delete;
        Serializer& operator=(const Serializer&) = delete;
    };

    // Implementation

    template <typename T> requires TriviallySerializable<T> void Serializer::write(const T& obj, const char* name)
    {
        this->beginObject(name);
        obj.serialize(*this);
        this->endObject();
    }

    template <typename T, typename R, typename I>
    requires TriviallySerializable<T> void Serializer::write(const T& obj, SerializationMap<R, I>&, const char* name)
    {
        this->write(obj, name);
    }

    template <typename T, typename R, typename I>
    requires MapSerializable<T, R, I> void Serializer::write(const T& obj, SerializationMap<R, I>& map,
                                                             const char* name)
    {
        this->beginObject(name);
        obj.serialize(*this, map);
        this->endObject();
    }

    template <typename T, typename R, typename I>
    void Serializer::write(const T& obj, SerializationMap<R, I>&, const char* name)
    {
        this->write(obj, name);
    }

    template <typename T> void Serializer::write(const std::vector<T>& vec, const char* name)
    {
        this->beginArray(vec.size(), name);
        for (const auto& obj : vec)
            this->write(obj);
        this->endArray();
    }

    template <typename T> void Serializer::write(const T* ptr, size_t length, const char* name)
    {
        this->beginArray(length, name);
        for (size_t i = 0; i < length; ++i)
            this->write(ptr[i]);
        this->endArray();
    }

    template <typename K, typename V> void Serializer::write(const std::unordered_map<K, V>& dic, const char* name)
    {
        this->beginDictionary(dic.size(), name);
        for (const auto& pair : dic)
        {
            this->write(pair.first);
            this->write(pair.second);
        }
        this->endDictionary();
    }

    template <typename T, typename R, typename I>
    void Serializer::write(const std::vector<T>& vec, SerializationMap<R, I>& map, const char* name)
    {
        this->beginArray(vec.size(), name);
        for (const auto& obj : vec)
            this->write(obj, map);
        this->endArray();
    }

    template <typename T, typename R, typename I>
    void Serializer::write(const T* ptr, size_t length, SerializationMap<R, I>& map, const char* name)
    {
        this->beginArray(length, name);
        for (size_t i = 0; i < length; ++i)
            this->write(ptr[i], map);
        this->endArray();
    }

    template <typename K, typename V, typename R, typename I>
    void Serializer::write(const std::unordered_map<K, V>& dic, SerializationMap<R, I>& map, const char* name)
    {
        this->beginDictionary(dic.size(), name);
        for (const auto& pair : dic)
        {
            this->write(pair.first, map);
            this->write(pair.second, map);
        }
        this->endDictionary();
    }
} // namespace cubos::memory

#endif // CUBOS_MEMORY_SERIALIZER_HPP
