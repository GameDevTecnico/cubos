#ifndef CUBOS_MEMORY_SERIALIZER_HPP
#define CUBOS_MEMORY_SERIALIZER_HPP

#include <cubos/memory/stream.hpp>

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
    class Serializer;

    /// Concept for serializable objects which are trivial to serialize.
    template <typename T>
    concept TriviallySerializable = requires(T obj, Serializer& s)
    {
        {
            obj.serialize(s)
            } -> std::same_as<void>;
    };

    /// Concept for serializable objects which require a context to be serialized.
    template <typename T, typename TCtx>
    concept ContextSerializable = requires(T obj, Serializer& s, TCtx ctx)
    {
        {
            obj.serialize(s, ctx)
            } -> std::same_as<void>;
    };

    /// Abstract class for serializing data.
    class Serializer
    {
    public:
        /// @param stream The stream to serialize to.
        Serializer(Stream& stream);
        virtual ~Serializer() = default;

        /// Flushes the serializer and writes the data to the stream.
        virtual void flush();

        /// Serializes a signed 8 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(int8_t value, const char* name) = 0;

        /// Serializes an signed 16 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(int16_t value, const char* name) = 0;

        /// Serializes an signed 32 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(int32_t value, const char* name) = 0;

        /// Serializes an signed 64 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(int64_t value, const char* name) = 0;

        /// Serializes an unsigned 8 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(uint8_t value, const char* name) = 0;

        /// Serializes an unsigned 16 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(uint16_t value, const char* name) = 0;

        /// Serializes an unsigned 32 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(uint32_t value, const char* name) = 0;

        /// Serializes an unsigned 64 bit integer.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(uint64_t value, const char* name) = 0;

        /// Serializes a float.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(float value, const char* name) = 0;

        /// Serializes a double.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(double value, const char* name) = 0;

        /// Serializes a boolean.
        /// @param value The value to serialize.
        /// @param name The name of the value (optional).
        virtual void write(bool value, const char* name) = 0;

        /// Serializes a string.
        /// @param str The string to serialize.
        /// @param name The name of the value (optional).
        virtual void write(const char* str, const char* name) = 0;

        /// Serializes a string.
        /// @param str The string to serialize.
        /// @param name The name of the value (optional).
        void write(const std::string& str, const char* name);

        /// Serializes a 2D vector.
        /// @tparam T The type of the vector.
        /// @param vec The vector to serialize.
        /// @param name The name of the value (optional).
        template <typename T> void write(const glm::tvec2<T>& vec, const char* name);

        /// Serializes a 3D vector.
        /// @tparam T The type of the vector.
        /// @param vec The vector to serialize.
        /// @param name The name of the value (optional).
        template <typename T> void write(const glm::tvec3<T>& vec, const char* name);

        /// Serializes a 4D vector.
        /// @tparam T The type of the vector.
        /// @param vec The vector to serialize.
        /// @param name The name of the value (optional).
        template <typename T> void write(const glm::tvec4<T>& vec, const char* name);

        /// Serializes a quaternion.
        /// @tparam T The type of the quaternion.
        /// @param quat The quaternion to serialize.
        /// @param name The name of the value (optional).
        template <typename T> void write(const glm::tquat<T>& quat, const char* name);

        /// Serializes a object.
        /// @tparam T The type of the object.
        /// @param obj The object to serialize.
        /// @param name The name of the object (optional).
        template <typename T>
        requires TriviallySerializable<T>
        void write(const T& obj, const char* name);

        /// Serializes a object.
        /// @tparam T The type of the object.
        /// @tparam TCtx The type of the context.
        /// @param obj The object to serialize.
        /// @param ctx The context required to serialize.
        /// @param name The name of the object (optional).
        template <typename T, typename TCtx>
        requires TriviallySerializable<T>
        void write(const T& obj, TCtx ctx, const char* name);

        /// Serializes a object which requires context to be serialized.
        /// @tparam T The type of the object.
        /// @tparam TCtx The type of the context.
        /// @param obj The object to serialize.
        /// @param ctx The context required to serialize.
        /// @param name The name of the object (optional).
        template <typename T, typename TCtx>
        requires ContextSerializable<T, TCtx>
        void write(const T& obj, TCtx ctx, const char* name);

        /// Serializes a object.
        /// @tparam T The type of the object.
        /// @tparam TCtx The type of the context.
        /// @param obj The object to serialize.
        /// @param ctx The context required to serialize.
        /// @param name The name of the object (optional).
        template <typename T, typename TCtx> void write(const T& obj, TCtx ctx, const char* name);

        /// Serializes a vector.
        /// @tparam T The type of the vector.
        /// @param vec The vector to serialize.
        /// @param name The name of the vector (optional).
        template <typename T> void write(const std::vector<T>& vec, const char* name);

        /// Serializes a unordered map.
        /// @tparam K The key type.
        /// @tparam V The value type.
        /// @param dic The map to serialize.
        /// @param name The name of the map (optional).
        template <typename K, typename V> void write(const std::unordered_map<K, V>& dic, const char* name);

        /// Serializes a vector.
        /// @tparam T The type of the vector.
        /// @tparam TCtx The type of the context.
        /// @param vec The vector to serialize.
        /// @param ctx The context required to serialize.
        /// @param name The name of the vector (optional).
        template <typename T, typename TCtx> void write(const std::vector<T>& vec, TCtx ctx, const char* name);

        /// Serializes a unordered map.
        /// @tparam K The key type.
        /// @tparam V The value type.
        /// @tparam TCtx The type of the context.
        /// @param dic The map to serialize.
        /// @param ctx The context required to serialize.
        /// @param name The name of the map (optional).
        template <typename K, typename V, typename TCtx>
        void write(const std::unordered_map<K, V>& dic, TCtx ctx, const char* name);

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

    protected:
        Stream& stream; ///< Stream used by the serializer.

    private:
        Serializer(const Serializer&) = delete;
        Serializer& operator=(const Serializer&) = delete;
    };

    // Implementation

    template <typename T> void Serializer::write(const glm::tvec2<T>& vec, const char* value)
    {
        this->beginObject(value);
        this->write(vec.x, "x");
        this->write(vec.y, "y");
        this->endObject();
    }

    template <typename T> void Serializer::write(const glm::tvec3<T>& vec, const char* value)
    {
        this->beginObject(value);
        this->write(vec.x, "x");
        this->write(vec.y, "y");
        this->write(vec.z, "z");
        this->endObject();
    }

    template <typename T> void Serializer::write(const glm::tvec4<T>& vec, const char* value)
    {
        this->beginObject(value);
        this->write(vec.x, "x");
        this->write(vec.y, "y");
        this->write(vec.z, "z");
        this->write(vec.w, "w");
        this->endObject();
    }

    template <typename T> void Serializer::write(const glm::tquat<T>& quat, const char* value)
    {
        this->beginObject(value);
        this->write(quat.w, "w");
        this->write(quat.x, "x");
        this->write(quat.y, "y");
        this->write(quat.z, "z");
        this->endObject();
    }

    template <typename T>
    requires TriviallySerializable<T>
    void Serializer::write(const T& obj, const char* name)
    {
        this->beginObject(name);
        obj.serialize(*this);
        this->endObject();
    }

    template <typename T, typename TCtx>
    requires TriviallySerializable<T>
    void Serializer::write(const T& obj, TCtx ctx, const char* name)
    {
        this->write(obj, name);
    }

    template <typename T, typename TCtx>
    requires ContextSerializable<T, TCtx>
    void Serializer::write(const T& obj, TCtx ctx, const char* name)
    {
        this->beginObject(name);
        obj.serialize(*this, ctx);
        this->endObject();
    }

    template <typename T, typename TCtx> void Serializer::write(const T& obj, TCtx, const char* name)
    {
        this->write(obj, name);
    }

    template <typename T> void Serializer::write(const std::vector<T>& vec, const char* name)
    {
        this->beginArray(vec.size(), name);
        for (const auto& obj : vec)
            this->write(obj, nullptr);
        this->endArray();
    }

    template <typename K, typename V> void Serializer::write(const std::unordered_map<K, V>& dic, const char* name)
    {
        this->beginDictionary(dic.size(), name);
        for (const auto& pair : dic)
        {
            this->write(pair.first, nullptr);
            this->write(pair.second, nullptr);
        }
        this->endDictionary();
    }

    template <typename T, typename TCtx> void Serializer::write(const std::vector<T>& vec, TCtx ctx, const char* name)
    {
        this->beginArray(vec.size(), name);
        for (const auto& obj : vec)
            this->write(obj, ctx, nullptr);
        this->endArray();
    }

    template <typename K, typename V, typename TCtx>
    void Serializer::write(const std::unordered_map<K, V>& dic, TCtx ctx, const char* name)
    {
        this->beginDictionary(dic.size(), name);
        for (const auto& pair : dic)
        {
            this->write(pair.first, ctx, nullptr);
            this->write(pair.second, ctx, nullptr);
        }
        this->endDictionary();
    }
} // namespace cubos::memory

#endif // CUBOS_MEMORY_SERIALIZER_HPP
