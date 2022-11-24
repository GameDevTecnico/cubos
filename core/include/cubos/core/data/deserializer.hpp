#ifndef CUBOS_CORE_DATA_DESERIALIZER_HPP
#define CUBOS_CORE_DATA_DESERIALIZER_HPP

#include <cubos/core/memory/stream.hpp>
#include <cubos/core/log.hpp>

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
    class Deserializer;

    /// Concept for deserializable objects which are trivial to deserialize.
    template <typename T>
    concept TriviallyDeserializable = requires(Deserializer& s, T& obj)
    {
        {
            deserialize(s, obj)
            } -> std::same_as<void>;
    };

    /// Concept for deserializable objects which require a context to be deserialized.
    template <typename T, typename TCtx>
    concept ContextDeserializable = requires(Deserializer& s, T& obj, TCtx&& ctx)
    {
        {
            deserialize(s, obj, ctx)
            } -> std::same_as<void>;
    };

    /// Abstract class for deserializing data.
    class Deserializer
    {
    public:
        Deserializer();
        virtual ~Deserializer() = default;

        /// Deserializes a signed 8 bit integer.
        /// @param value The value to deserialize.
        virtual void readI8(int8_t& value) = 0;

        /// Deserializes a signed 16 bit integer.
        /// @param value The value to deserialize.
        virtual void readI16(int16_t& value) = 0;

        /// Deserializes a signed 32 bit integer.
        /// @param value The value to deserialize.
        virtual void readI32(int32_t& value) = 0;

        /// Deserializes a signed 64 bit integer.
        /// @param value The value to deserialize.
        virtual void readI64(int64_t& value) = 0;

        /// Deserializes an unsigned 8 bit integer.
        /// @param value The value to deserialize.
        virtual void readU8(uint8_t& value) = 0;

        /// Deserializes an unsigned 16 bit integer.
        /// @param value The value to deserialize.
        virtual void readU16(uint16_t& value) = 0;

        /// Deserializes an unsigned 32 bit integer.
        /// @param value The value to deserialize.
        virtual void readU32(uint32_t& value) = 0;

        /// Deserializes an unsigned 64 bit integer.
        /// @param value The value to deserialize.
        virtual void readU64(uint64_t& value) = 0;

        /// Deserializes a float.
        /// @param value The value to deserialize.
        virtual void readF32(float& value) = 0;

        /// Deserializes a double.
        /// @param value The value to deserialize.
        virtual void readF64(double& value) = 0;

        /// Deserializes a boolean.
        /// @param value The value to deserialize.
        virtual void readBool(bool& value) = 0;

        /// Deserializes a string.
        /// @param value The value to deserialize.
        virtual void readString(std::string& value) = 0;

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
        requires(TriviallyDeserializable<T> && !ContextDeserializable<T, TCtx>) void read(T& obj, TCtx&& ctx);

        /// Deserializes a object.
        /// @tparam T The type of the object.
        /// @tparam TCtx The type of the context.
        /// @param obj The object to deserialize.
        /// @param ctx The context required to deserialize.
        template <typename T, typename TCtx>
        requires ContextDeserializable<T, TCtx>
        void read(T& obj, TCtx&& ctx);

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

        /// Checks if the deserializer has failed.
        bool failed() const;

    protected:
        bool failBit; ///< Indicates if the deserializer has failed.

    private:
        Deserializer(const Deserializer&) = delete;
        Deserializer& operator=(const Deserializer&) = delete;
    };

    // Implementation

    template <typename T>
    requires TriviallyDeserializable<T>
    void Deserializer::read(T& obj)
    {
        deserialize(*this, obj);
    }

    template <typename T, typename TCtx>
    requires(TriviallyDeserializable<T> && !ContextDeserializable<T, TCtx>) void Deserializer::read(T& obj, TCtx&&)
    {
        this->read(obj);
    }

    template <typename T, typename TCtx>
    requires ContextDeserializable<T, TCtx>
    void Deserializer::read(T& obj, TCtx&& ctx)
    {
        deserialize(*this, obj, ctx);
    }

    inline void deserialize(Deserializer& d, int8_t& value)
    {
        d.readI8(value);
    }

    inline void deserialize(Deserializer& d, int16_t& value)
    {
        d.readI16(value);
    }

    inline void deserialize(Deserializer& d, int32_t& value)
    {
        d.readI32(value);
    }

    inline void deserialize(Deserializer& d, int64_t& value)
    {
        d.readI64(value);
    }

    inline void deserialize(Deserializer& d, uint8_t& value)
    {
        d.readU8(value);
    }

    inline void deserialize(Deserializer& d, uint16_t& value)
    {
        d.readU16(value);
    }

    inline void deserialize(Deserializer& d, uint32_t& value)
    {
        d.readU32(value);
    }

    inline void deserialize(Deserializer& d, uint64_t& value)
    {
        d.readU64(value);
    }

    inline void deserialize(Deserializer& d, float& value)
    {
        d.readF32(value);
    }

    inline void deserialize(Deserializer& d, double& value)
    {
        d.readF64(value);
    }

    inline void deserialize(Deserializer& d, bool& value)
    {
        d.readBool(value);
    }

    inline void deserialize(Deserializer& d, std::vector<bool>::reference& value)
    {
        bool boolean;
        d.readBool(boolean);
        value = boolean;
    }

    inline void deserialize(Deserializer& d, std::string& value)
    {
        d.readString(value);
    }

    template <typename T> void deserialize(Deserializer& d, glm::tvec2<T>& value)
    {
        d.beginObject();
        d.read(value.x);
        d.read(value.y);
        d.endObject();
    }

    template <typename T> void deserialize(Deserializer& d, glm::tvec3<T>& value)
    {
        d.beginObject();
        d.read(value.x);
        d.read(value.y);
        d.read(value.z);
        d.endObject();
    }

    template <typename T> void deserialize(Deserializer& d, glm::tvec4<T>& value)
    {
        d.beginObject();
        d.read(value.x);
        d.read(value.y);
        d.read(value.z);
        d.read(value.w);
        d.endObject();
    }

    template <typename T> void deserialize(Deserializer& d, glm::tquat<T>& value)
    {
        d.beginObject();
        d.read(value.x);
        d.read(value.y);
        d.read(value.z);
        d.read(value.w);
        d.endObject();
    }

    template <typename T> void deserialize(Deserializer& d, glm::tmat4x4<T>& mat)
    {
        d.beginObject();
        d.read(mat[0]);
        d.read(mat[1]);
        d.read(mat[2]);
        d.read(mat[3]);
        d.endObject();
    }

    template <typename T>
    requires TriviallyDeserializable<T>
    void deserialize(Deserializer& d, std::vector<T>& vec)
    {
        size_t length = d.beginArray();
        vec.resize(length);
        for (size_t i = 0; i < length; ++i)
            d.read(vec[i]);
        d.endArray();
    }

    template <> inline void deserialize(Deserializer& d, std::vector<bool>& vec)
    {
        size_t length = d.beginArray();
        vec.resize(length);
        for (size_t i = 0; i < length; ++i)
        {
            bool val;
            d.read(val);
            vec[i] = val;
        }
        d.endArray();
    }

    template <typename K, typename V>
    requires TriviallyDeserializable<K> && TriviallyDeserializable<V>
    void deserialize(Deserializer& d, std::unordered_map<K, V>& dic)
    {
        size_t length = d.beginDictionary();
        dic.clear();
        dic.reserve(length);
        for (size_t i = 0; i < length; ++i)
        {
            K key;
            V value;
            d.read(key);
            d.read(value);
            dic[key] = value;
        }
        d.endDictionary();
    }

    template <typename T, typename TCtx>
    requires ContextDeserializable<T, TCtx>
    void deserialize(Deserializer& d, std::vector<T>& vec, TCtx&& ctx)
    {
        size_t length = d.beginArray();
        vec.resize(length);
        for (size_t i = 0; i < length; ++i)
            d.read(vec[i], ctx);
        d.endArray();
    }

    template <typename K, typename V, typename TCtx>
    requires(TriviallyDeserializable<K> || ContextDeserializable<K, TCtx>) &&
        (TriviallyDeserializable<V> ||
         ContextDeserializable<V, TCtx>)void deserialize(Deserializer& d, std::unordered_map<K, V>& dic, TCtx&& ctx)
    {
        size_t length = d.beginDictionary();
        dic.clear();
        dic.reserve(length);
        for (size_t i = 0; i < length; ++i)
        {
            K key;
            V value;
            d.read(key, ctx);
            d.read(value, ctx);
            dic[key] = value;
        }
        d.endDictionary();
    }
} // namespace cubos::core::data

#endif // CUBOS_CORE_DATA_DESERIALIZER_HPP
