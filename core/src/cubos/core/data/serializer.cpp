#include <cubos/core/data/serializer.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>

using namespace cubos::core::data;

Serializer::Serializer()
{
    this->failBit = false;
}

void Serializer::flush()
{
    // Do nothing.
}

bool Serializer::failed() const
{
    return this->failBit;
}

// Implementation of serialize() for primitive types.

template <>
void cubos::core::data::serialize<int8_t>(Serializer& ser, const int8_t& val, const char* name)
{
    ser.writeI8(val, name);
}

template <>
void cubos::core::data::serialize<int16_t>(Serializer& ser, const int16_t& val, const char* name)
{
    ser.writeI16(val, name);
}

template <>
void cubos::core::data::serialize<int32_t>(Serializer& ser, const int32_t& val, const char* name)
{
    ser.writeI32(val, name);
}

template <>
void cubos::core::data::serialize<int64_t>(Serializer& ser, const int64_t& val, const char* name)
{
    ser.writeI64(val, name);
}

template <>
void cubos::core::data::serialize<uint8_t>(Serializer& ser, const uint8_t& val, const char* name)
{
    ser.writeU8(val, name);
}

template <>
void cubos::core::data::serialize<uint16_t>(Serializer& ser, const uint16_t& val, const char* name)
{
    ser.writeU16(val, name);
}

template <>
void cubos::core::data::serialize<uint32_t>(Serializer& ser, const uint32_t& val, const char* name)
{
    ser.writeU32(val, name);
}

template <>
void cubos::core::data::serialize<uint64_t>(Serializer& ser, const uint64_t& val, const char* name)
{
    ser.writeU64(val, name);
}

template <>
void cubos::core::data::serialize<float>(Serializer& ser, const float& val, const char* name)
{
    ser.writeF32(val, name);
}

template <>
void cubos::core::data::serialize<double>(Serializer& ser, const double& val, const char* name)
{
    ser.writeF64(val, name);
}

template <>
void cubos::core::data::serialize<bool>(Serializer& ser, const bool& val, const char* name)
{
    ser.writeBool(val, name);
}

template <>
void cubos::core::data::serialize<const char*>(Serializer& ser, const char* const& val, const char* name)
{
    ser.writeString(val, name);
}

template <>
void cubos::core::data::serialize<std::string>(Serializer& ser, const std::string& val, const char* name)
{
    ser.writeString(val.c_str(), name);
}

// Implementation of serialize() for GLM types.
// A macro is used to implement for each type without repeating code.
#define IMPL_SERIALIZE_GLM(T)                                                                                          \
    template <>                                                                                                        \
    void cubos::core::data::serialize<glm::tvec2<T>>(Serializer & ser, const glm::tvec2<T>& val, const char* name)     \
    {                                                                                                                  \
        ser.beginObject(name);                                                                                         \
        ser.write(val.x, "x");                                                                                         \
        ser.write(val.y, "y");                                                                                         \
        ser.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::serialize<glm::tvec3<T>>(Serializer & ser, const glm::tvec3<T>& val, const char* name)     \
    {                                                                                                                  \
        ser.beginObject(name);                                                                                         \
        ser.write(val.x, "x");                                                                                         \
        ser.write(val.y, "y");                                                                                         \
        ser.write(val.z, "z");                                                                                         \
        ser.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::serialize<glm::tvec4<T>>(Serializer & ser, const glm::tvec4<T>& val, const char* name)     \
    {                                                                                                                  \
        ser.beginObject(name);                                                                                         \
        ser.write(val.x, "x");                                                                                         \
        ser.write(val.y, "y");                                                                                         \
        ser.write(val.z, "z");                                                                                         \
        ser.write(val.w, "w");                                                                                         \
        ser.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::serialize<glm::tquat<T>>(Serializer & ser, const glm::tquat<T>& val, const char* name)     \
    {                                                                                                                  \
        ser.beginObject(name);                                                                                         \
        ser.write(val.w, "w");                                                                                         \
        ser.write(val.x, "x");                                                                                         \
        ser.write(val.y, "y");                                                                                         \
        ser.write(val.z, "z");                                                                                         \
        ser.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::serialize<glm::tmat4x4<T>>(Serializer & ser, const glm::tmat4x4<T>& val, const char* name) \
    {                                                                                                                  \
        ser.beginObject(name);                                                                                         \
        ser.write(val[0], "0");                                                                                        \
        ser.write(val[1], "1");                                                                                        \
        ser.write(val[2], "2");                                                                                        \
        ser.write(val[3], "3");                                                                                        \
        ser.endObject();                                                                                               \
    }

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
