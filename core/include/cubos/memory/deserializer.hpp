#ifndef CUBOS_CORE_MEMORY_DESERIALIZER_HPP
#define CUBOS_CORE_MEMORY_DESERIALIZER_HPP

#include <cubos/memory/stream.hpp>
#include <cubos/log.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <concepts>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cubos::core::memory
{
    class Deserializer;

    /// Concept for deserializable objects which are trivial to deserialize.
    template <typename T>
    concept TriviallyDeserializable = requires(T obj, Deserializer& s)
    {
        {
            obj.deserialize(s)
            } -> std::same_as<void>;
    };

    /// Concept for deserializable objects which require a context to be deserialized.
    template <typename T, typename TCtx>
    concept ContextDeserializable = requires(T obj, Deserializer& s, TCtx ctx)
    {
        {
            obj.deserialize(s, ctx)
            } -> std::same_as<void>;
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

        /// Deserializes a boolean.
        /// @param value The value to deserialize.
        virtual void read(bool& value) = 0;

        /// Deserializes a string.
        /// @param value The value to deserialize.
        virtual void read(std::string& value) = 0;

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
        template <typename T>
        requires TriviallyDeserializable<T>
        void read(T& obj);

        /// Deserializes a object.
        /// @tparam T The type of the object.
        /// @tparam TCtx The type of the context.
        /// @param obj The object to deserialize.
        /// @param ctx The context required to deserialize.
        template <typename T, typename TCtx>
        requires(TriviallyDeserializable<T> && !ContextDeserializable<T, TCtx>) void read(T& obj, TCtx ctx);

        /// Deserializes a object.
        /// @tparam T The type of the object.
        /// @tparam TCtx The type of the context.
        /// @param obj The object to deserialize.
        /// @param ctx The context required to deserialize.
        template <typename T, typename TCtx>
        requires ContextDeserializable<T, TCtx>
        void read(T& obj, TCtx ctx);

        /// Deserializes a vector.
        /// @tparam T The type of the vector.
        /// @param vec The vector to deserialize.
        template <typename T> void read(std::vector<T>& vec);

        /// Deserializes a unordered map.
        /// @tparam K The key type.
        /// @tparam V The value type.
        /// @param dic The map to deserialize.
        template <typename K, typename V> void read(std::unordered_map<K, V>& dic);

        /// Deserializes a vector.
        /// @tparam T The type of the vector.
        /// @tparam TCtx The type of the context.
        /// @param vec The vector to deserialize.
        /// @param ctx The context required to deserialize.
        template <typename T, typename TCtx> void read(std::vector<T>& vec, TCtx ctx);

        /// Deserializes a unordered map.
        /// @tparam K The key type.
        /// @tparam V The value type.
        /// @tparam TCtx The type of the context.
        /// @param dic The map to deserialize.
        /// @param ctx The context required to deserialize.
        template <typename K, typename V, typename TCtx> void read(std::unordered_map<K, V>& dic, TCtx ctx);

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

    protected:
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

    template <typename T>
    requires TriviallyDeserializable<T>
    void Deserializer::read(T& obj)
    {
        this->beginObject();
        obj.deserialize(*this);
        this->endObject();
    }

    template <typename T, typename TCtx>
    requires(TriviallyDeserializable<T> && !ContextDeserializable<T, TCtx>) void Deserializer::read(T& obj, TCtx)
    {
        this->read(obj);
    }

    template <typename T, typename TCtx>
    requires ContextDeserializable<T, TCtx>
    void Deserializer::read(T& obj, TCtx ctx)
    {
        this->beginObject();
        obj.deserialize(*this, ctx);
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

    template <typename T, typename TCtx> void Deserializer::read(std::vector<T>& vec, TCtx ctx)
    {
        size_t length = this->beginArray();
        vec.resize(length);
        for (size_t i = 0; i < length; ++i)
            this->read(vec[i], ctx);
        this->endArray();
    }

    template <typename K, typename V, typename TCtx> void Deserializer::read(std::unordered_map<K, V>& dic, TCtx ctx)
    {
        size_t length = this->beginDictionary();
        for (size_t i = 0; i < length; ++i)
        {
            K key;
            V value;
            this->read(key, ctx);
            this->read(value, ctx);
            dic[key] = value;
        }
        this->endDictionary();
    }

} // namespace cubos::core::memory

#endif // CUBOS_CORE_MEMORY_DESERIALIZER_HPP
