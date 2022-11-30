#ifndef CUBOS_CORE_DATA_SERIALIZER_HPP
#define CUBOS_CORE_DATA_SERIALIZER_HPP

#include <cubos/core/memory/stream.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <concepts>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace cubos::core::data
{
    class Serializer;

    /// Concept for serializable objects which are trivial to serialize.
    template <typename T>
    concept TriviallySerializable = requires(Serializer& s, const T& obj, const char* name)
    {
        {
            serialize(s, obj, name)
            } -> std::same_as<void>;
    };

    /// Concept for serializable objects which require a context to be serialized.
    template <typename T, typename TCtx>
    concept ContextSerializable = requires(Serializer& s, const T& obj, TCtx ctx, const char* name)
    {
        {
            serialize(s, obj, ctx, name)
            } -> std::same_as<void>;
    };

    /// Abstract class for serializing data.
    class Serializer
    {
    public:
        Serializer();
        virtual ~Serializer() = default;

        /// Flushes the serializer and writes the data to the stream.
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
        requires(TriviallySerializable<T> && !ContextSerializable<T, TCtx>) void write(const T& obj, TCtx ctx,
                                                                                       const char* name);

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

    protected:
        bool failBit; ///< Indicates if the serializer failed.

    private:
        Serializer(const Serializer&) = delete;
        Serializer& operator=(const Serializer&) = delete;
    };

    // Implementation

    template <typename T>
    requires TriviallySerializable<T>
    void Serializer::write(const T& obj, const char* name)
    {
        serialize(*this, obj, name);
    }

    template <typename T, typename TCtx>
    requires(TriviallySerializable<T> && !ContextSerializable<T, TCtx>) void Serializer::write(const T& obj, TCtx ctx,
                                                                                               const char* name)
    {
        this->write(obj, name);
    }

    template <typename T, typename TCtx>
    requires ContextSerializable<T, TCtx>
    void Serializer::write(const T& obj, TCtx ctx, const char* name)
    {
        serialize(*this, obj, ctx, name);
    }

    inline void serialize(Serializer& s, const int8_t& v, const char* name)
    {
        s.writeI8(v, name);
    }

    inline void serialize(Serializer& s, const int16_t& v, const char* name)
    {
        s.writeI16(v, name);
    }

    inline void serialize(Serializer& s, const int32_t& v, const char* name)
    {
        s.writeI32(v, name);
    }

    inline void serialize(Serializer& s, const int64_t& v, const char* name)
    {
        s.writeI64(v, name);
    }

    inline void serialize(Serializer& s, const uint8_t& v, const char* name)
    {
        s.writeU8(v, name);
    }

    inline void serialize(Serializer& s, const uint16_t& v, const char* name)
    {
        s.writeU16(v, name);
    }

    inline void serialize(Serializer& s, const uint32_t& v, const char* name)
    {
        s.writeU32(v, name);
    }

    inline void serialize(Serializer& s, const uint64_t& v, const char* name)
    {
        s.writeU64(v, name);
    }

    inline void serialize(Serializer& s, const float& v, const char* name)
    {
        s.writeF32(v, name);
    }

    inline void serialize(Serializer& s, const double& v, const char* name)
    {
        s.writeF64(v, name);
    }

    inline void serialize(Serializer& s, const bool& v, const char* name)
    {
        s.writeBool(v, name);
    }

    inline void serialize(Serializer& s, const char* v, const char* name)
    {
        s.writeString(v, name);
    }

    inline void serialize(Serializer& s, const std::string& v, const char* name)
    {
        s.writeString(v.c_str(), name);
    }

    template <typename T> void serialize(Serializer& s, const glm::tvec2<T>& vec, const char* name)
    {
        s.beginObject(name);
        s.write(vec.x, "x");
        s.write(vec.y, "y");
        s.endObject();
    }

    template <typename T> void serialize(Serializer& s, const glm::tvec3<T>& vec, const char* name)
    {
        s.beginObject(name);
        s.write(vec.x, "x");
        s.write(vec.y, "y");
        s.write(vec.z, "z");
        s.endObject();
    }

    template <typename T> void serialize(Serializer& s, const glm::tvec4<T>& vec, const char* name)
    {
        s.beginObject(name);
        s.write(vec.x, "x");
        s.write(vec.y, "y");
        s.write(vec.z, "z");
        s.write(vec.w, "w");
        s.endObject();
    }

    template <typename T> void serialize(Serializer& s, const glm::tquat<T>& quat, const char* name)
    {
        s.beginObject(name);
        s.write(quat.w, "w");
        s.write(quat.x, "x");
        s.write(quat.y, "y");
        s.write(quat.z, "z");
        s.endObject();
    }

    template <typename T>
    requires TriviallySerializable<T>
    void serialize(Serializer& s, const std::vector<T>& vec, const char* name)
    {
        s.beginArray(vec.size(), name);
        for (const auto& obj : vec)
            s.write(obj, nullptr);
        s.endArray();
    }

    template <typename T, typename TCtx>
    requires ContextSerializable<T, TCtx>
    void serialize(Serializer& s, const std::vector<T>& vec, TCtx ctx, const char* name)
    {
        s.beginArray(vec.size(), name);
        for (const auto& obj : vec)
            s.write(obj, ctx, nullptr);
        s.endArray();
    }

    template <typename K, typename V>
    requires TriviallySerializable<K> && TriviallySerializable<V>
    void serialize(Serializer& s, const std::unordered_map<K, V>& dic, const char* name)
    {
        s.beginDictionary(dic.size(), name);
        for (const auto& pair : dic)
        {
            s.write(pair.first, nullptr);
            s.write(pair.second, nullptr);
        }
        s.endDictionary();
    }

    template <typename K, typename V, typename TCtx>
    requires(TriviallySerializable<K> || ContextSerializable<K, TCtx>) &&
        (TriviallySerializable<V> || ContextSerializable<V, TCtx>)void serialize(Serializer& s,
                                                                                 const std::unordered_map<K, V>& dic,
                                                                                 TCtx ctx, const char* name)
    {
        s.beginDictionary(dic.size(), name);
        for (const auto& pair : dic)
        {
            s.write(pair.first, ctx, nullptr);
            s.write(pair.second, ctx, nullptr);
        }
        s.endDictionary();
    }
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_SERIALIZER_HPP
