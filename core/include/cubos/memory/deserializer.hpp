#ifndef CUBOS_MEMORY_DESERIALIZER_HPP
#define CUBOS_MEMORY_DESERIALIZER_HPP

#include <cubos/memory/stream.hpp>
#include <cubos/memory/serialization_map.hpp>
#include <cubos/log.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <concepts>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cubos::memory
{
    class Deserializer;

    /// Concept for deserializable objects which are trivial to deserialize.
    template <typename T> concept TriviallyDeserializable = requires(T obj, Deserializer& s)
    {
        {
            obj.deserialize(s)
        }
        ->std::same_as<void>;
    };

    /// Concept for deserializable objects which require a serialization map.
    template <typename T, typename R, typename I>
    concept MapDeserializable = requires(T obj, Deserializer& s, SerializationMap<R, I>& map)
    {
        {
            obj.deserialize(s, map)
        }
        ->std::same_as<void>;
    };

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

        /// Deserializes a 2D vector.
        /// @tparam T The type of the vector.
        /// @param value The value to deserialize.
        template <typename T> void read(glm::tvec2<T>& value);

        /// Deserializes a 3D vector.
        /// @tparam T The type of the vector.
        /// @param value The value to deserialize.
        template <typename T> void read(glm::tvec3<T>& value);

        /// Deserializes a 4D vector.
        /// @tparam T The type of the vector.
        /// @param value The value to deserialize.
        template <typename T> void read(glm::tvec4<T>& value);

        /// Deserializes a quaternion.
        /// @tparam T The type of the vector.
        /// @param value The value to deserialize.
        template <typename T> void read(glm::tquat<T>& value);

        /// Deserializes a object.
        /// @tparam T The type of the object.
        /// @param obj The object to deserialize.
        template <typename T> requires TriviallyDeserializable<T> void read(T& obj);

        /// Deserializes a object.
        /// @tparam T The type of the object.
        /// @tparam R The reference type.
        /// @tparam I The serialized identifier type.
        /// @param obj The object to deserialize.
        /// @param map The serialization map.
        template <typename T, typename R, typename I>
        requires TriviallyDeserializable<T> void read(T& obj, SerializationMap<R, I>& map);

        /// Deserializes a object.
        /// @tparam T The type of the object.
        /// @tparam R The reference type.
        /// @tparam I The serialized identifier type.
        /// @param obj The object to deserialize.
        /// @param map The serialization map.
        template <typename T, typename R, typename I>
        requires MapDeserializable<T, R, I> void read(T& obj, SerializationMap<R, I>& map);

        /// Deserializes a vector.
        /// @tparam T The type of the vector.
        /// @param vec The vector to deserialize.
        template <typename T> void read(std::vector<T>& vec);

        /// Deserializes an array.
        /// @tparam T The type of the array.
        /// @param ptr The array to deserialize.
        /// @param length The maximum length of the array.
        /// @return The number of elements read.
        template <typename T> size_t read(T* ptr, size_t length);

        /// Deserializes a unordered map.
        /// @tparam K The key type.
        /// @tparam V The value type.
        /// @param dic The map to deserialize.
        template <typename K, typename V> void read(std::unordered_map<K, V>& dic);

        /// Deserializes a vector.
        /// @tparam T The type of the vector.
        /// @tparam R The reference type.
        /// @tparam I The serialized identifier type.
        /// @param vec The vector to deserialize.
        /// @param map The serialization map.
        template <typename T, typename R, typename I> void read(std::vector<T>& vec, SerializationMap<R, I>& map);

        /// Deserializes an array.
        /// @tparam T The type of the array.
        /// @tparam R The reference type.
        /// @tparam I The serialized identifier type.
        /// @param ptr The array to deserialize.
        /// @param length The maximum length of the array.
        /// @param map The serialization map.
        /// @return The number of elements read.
        template <typename T, typename R, typename I> size_t read(T* ptr, size_t length, SerializationMap<R, I>& map);

        /// Deserializes a unordered map.
        /// @tparam K The key type.
        /// @tparam V The value type.
        /// @tparam R The reference type.
        /// @tparam I The serialized identifier type.
        /// @param dic The map to deserialize.
        /// @param map The serialization map.
        template <typename K, typename V, typename R, typename I>
        void read(std::unordered_map<K, V>& dic, SerializationMap<R, I>& map);

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

    // Implementation

    template <typename T> void Deserializer::read(glm::tvec2<T>& value)
    {
        this->beginObject();
        this->read(value.x);
        this->read(value.y);
        this->endObject();
    }

    template <typename T> void Deserializer::read(glm::tvec3<T>& value)
    {
        this->beginObject();
        this->read(value.x);
        this->read(value.y);
        this->read(value.z);
        this->endObject();
    }

    template <typename T> void Deserializer::read(glm::tvec4<T>& value)
    {
        this->beginObject();
        this->read(value.x);
        this->read(value.y);
        this->read(value.z);
        this->read(value.w);
        this->endObject();
    }

    template <typename T> void Deserializer::read(glm::tquat<T>& value)
    {
        this->beginObject();
        this->read(value.x);
        this->read(value.y);
        this->read(value.z);
        this->read(value.w);
        this->endObject();
    }

    template <typename T> requires TriviallyDeserializable<T> void Deserializer::read(T& obj)
    {
        this->beginObject();
        obj.deserialize(*this);
        this->endObject();
    }

    template <typename T, typename R, typename I>
    requires TriviallyDeserializable<T> void Deserializer::read(T& obj, SerializationMap<R, I>& map)
    {
        this->read(obj);
    }

    template <typename T, typename R, typename I>
    requires MapDeserializable<T, R, I> void Deserializer::read(T& obj, SerializationMap<R, I>& map)
    {
        this->beginObject();
        obj.deserialize(*this, map);
        this->endObject();
    }

    template <typename T> void Deserializer::read(std::vector<T>& vec)
    {
        size_t length = this->beginArray();
        vec.resize(length);
        for (size_t i = 0; i < length; ++i)
            this->read(vec[i]);
        this->endArray();
    }

    template <typename T> size_t Deserializer::read(T* ptr, size_t length)
    {
        size_t l = this->beginArray();
        if (length > l)
            length = l;
        else
            logWarning("Deserializer::read(T*, size_t) couldn't read the entire array because the array passed was too "
                       "small ({} < {})",
                       length, l);
        for (size_t i = 0; i < length; ++i)
            this->read(ptr[i]);
        this->endArray();
        return length;
    }

    template <typename K, typename V> void Deserializer::read(std::unordered_map<K, V>& dic)
    {
        size_t length = this->beginDictionary();
        for (size_t i = 0; i < length; ++i)
        {
            K key;
            V value;
            this->read(key);
            this->read(value);
            dic[key] = value;
        }
        this->endDictionary();
    }

    template <typename T, typename R, typename I>
    void Deserializer::read(std::vector<T>& vec, SerializationMap<R, I>& map)
    {
        size_t length = this->beginArray();
        vec.resize(length);
        for (size_t i = 0; i < length; ++i)
            this->read(vec[i], map);
        this->endArray();
    }

    template <typename T, typename R, typename I>
    size_t Deserializer::read(T* ptr, size_t length, SerializationMap<R, I>& map)
    {
        size_t l = this->beginArray();
        if (length > l)
            length = l;
        else
            logWarning("Deserializer::read(T*, size_t, SerializationMap<R, I>) couldn't read the entire array because "
                       "the array passed was too small ({} < {})",
                       length, l);
        for (size_t i = 0; i < length; ++i)
            this->read(ptr[i], map);
        this->endArray();
        return length;
    }

    template <typename K, typename V, typename R, typename I>
    void Deserializer::read(std::unordered_map<K, V>& dic, SerializationMap<R, I>& map)
    {
        size_t length = this->beginDictionary();
        for (size_t i = 0; i < length; ++i)
        {
            K key;
            V value;
            this->read(key, map);
            this->read(value, map);
            dic[key] = value;
        }
        this->endDictionary();
    }

} // namespace cubos::memory

#endif // CUBOS_MEMORY_DESERIALIZER_HPP
