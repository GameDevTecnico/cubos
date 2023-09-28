#include <string>

#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cubos/core/data/old/serializer.hpp>

using namespace cubos::core::data::old;

Serializer::Serializer()
{
    mFailBit = false;
}

void Serializer::flush()
{
    // Do nothing.
}

bool Serializer::failed() const
{
    return mFailBit;
}

// Implementation of serialize() for primitive types.

template <>
void cubos::core::data::old::serialize<int8_t>(Serializer& ser, const int8_t& obj, const char* name)
{
    ser.writeI8(obj, name);
}

template <>
void cubos::core::data::old::serialize<int16_t>(Serializer& ser, const int16_t& obj, const char* name)
{
    ser.writeI16(obj, name);
}

template <>
void cubos::core::data::old::serialize<int32_t>(Serializer& ser, const int32_t& obj, const char* name)
{
    ser.writeI32(obj, name);
}

template <>
void cubos::core::data::old::serialize<int64_t>(Serializer& ser, const int64_t& obj, const char* name)
{
    ser.writeI64(obj, name);
}

template <>
void cubos::core::data::old::serialize<uint8_t>(Serializer& ser, const uint8_t& obj, const char* name)
{
    ser.writeU8(obj, name);
}

template <>
void cubos::core::data::old::serialize<uint16_t>(Serializer& ser, const uint16_t& obj, const char* name)
{
    ser.writeU16(obj, name);
}

template <>
void cubos::core::data::old::serialize<uint32_t>(Serializer& ser, const uint32_t& obj, const char* name)
{
    ser.writeU32(obj, name);
}

template <>
void cubos::core::data::old::serialize<uint64_t>(Serializer& ser, const uint64_t& obj, const char* name)
{
    ser.writeU64(obj, name);
}

template <>
void cubos::core::data::old::serialize<float>(Serializer& ser, const float& obj, const char* name)
{
    ser.writeF32(obj, name);
}

template <>
void cubos::core::data::old::serialize<double>(Serializer& ser, const double& obj, const char* name)
{
    ser.writeF64(obj, name);
}

template <>
void cubos::core::data::old::serialize<bool>(Serializer& ser, const bool& obj, const char* name)
{
    ser.writeBool(obj, name);
}

template <>
void cubos::core::data::old::serialize<const char*>(Serializer& ser, const char* const& obj, const char* name)
{
    ser.writeString(obj, name);
}

template <>
void cubos::core::data::old::serialize<std::string>(Serializer& ser, const std::string& obj, const char* name)
{
    ser.writeString(obj.c_str(), name);
}

void cubos::core::data::old::serialize(Serializer& ser, std::vector<bool>::const_reference obj, const char* name)
{
    ser.writeBool(obj, name);
}

// Implementation of serialize() for GLM types.
// A macro is used to implement for each type without repeating code.
// NOLINTBEGIN(bugprone-macro-parentheses)
#define IMPL_SERIALIZE_GLM(T)                                                                                          \
    template <>                                                                                                        \
    void cubos::core::data::old::serialize<glm::tvec2<T>>(Serializer & ser, const glm::tvec2<T>& obj,                  \
                                                          const char* name)                                            \
    {                                                                                                                  \
        ser.beginObject(name);                                                                                         \
        ser.write(obj.x, "x");                                                                                         \
        ser.write(obj.y, "y");                                                                                         \
        ser.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::old::serialize<glm::tvec3<T>>(Serializer & ser, const glm::tvec3<T>& obj,                  \
                                                          const char* name)                                            \
    {                                                                                                                  \
        ser.beginObject(name);                                                                                         \
        ser.write(obj.x, "x");                                                                                         \
        ser.write(obj.y, "y");                                                                                         \
        ser.write(obj.z, "z");                                                                                         \
        ser.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::old::serialize<glm::tvec4<T>>(Serializer & ser, const glm::tvec4<T>& obj,                  \
                                                          const char* name)                                            \
    {                                                                                                                  \
        ser.beginObject(name);                                                                                         \
        ser.write(obj.x, "x");                                                                                         \
        ser.write(obj.y, "y");                                                                                         \
        ser.write(obj.z, "z");                                                                                         \
        ser.write(obj.w, "w");                                                                                         \
        ser.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::old::serialize<glm::tquat<T>>(Serializer & ser, const glm::tquat<T>& obj,                  \
                                                          const char* name)                                            \
    {                                                                                                                  \
        ser.beginObject(name);                                                                                         \
        ser.write(obj.w, "w");                                                                                         \
        ser.write(obj.x, "x");                                                                                         \
        ser.write(obj.y, "y");                                                                                         \
        ser.write(obj.z, "z");                                                                                         \
        ser.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::old::serialize<glm::tmat4x4<T>>(Serializer & ser, const glm::tmat4x4<T>& obj,              \
                                                            const char* name)                                          \
    {                                                                                                                  \
        ser.beginObject(name);                                                                                         \
        ser.write(obj[0], "0");                                                                                        \
        ser.write(obj[1], "1");                                                                                        \
        ser.write(obj[2], "2");                                                                                        \
        ser.write(obj[3], "3");                                                                                        \
        ser.endObject();                                                                                               \
    }
// NOLINTEND(bugprone-macro-parentheses)

IMPL_SERIALIZE_GLM(int8_t)
IMPL_SERIALIZE_GLM(int16_t)
IMPL_SERIALIZE_GLM(int32_t)
IMPL_SERIALIZE_GLM(int64_t)
IMPL_SERIALIZE_GLM(uint8_t)
IMPL_SERIALIZE_GLM(uint16_t)
IMPL_SERIALIZE_GLM(uint32_t)
IMPL_SERIALIZE_GLM(uint64_t)
IMPL_SERIALIZE_GLM(float)
IMPL_SERIALIZE_GLM(double)
IMPL_SERIALIZE_GLM(bool)
