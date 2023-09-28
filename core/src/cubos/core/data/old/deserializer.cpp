#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cubos/core/data/old/deserializer.hpp>

using namespace cubos::core::data::old;

Deserializer::Deserializer()
{
    mFailBit = false;
}

bool Deserializer::failed() const
{
    return mFailBit;
}

void Deserializer::fail()
{
    mFailBit = true;
}

// Implementation of deserialize() for primitive types.

template <>
void cubos::core::data::old::deserialize<int8_t>(Deserializer& des, int8_t& obj)
{
    des.readI8(obj);
}

template <>
void cubos::core::data::old::deserialize<int16_t>(Deserializer& des, int16_t& obj)
{
    des.readI16(obj);
}

template <>
void cubos::core::data::old::deserialize<int32_t>(Deserializer& des, int32_t& obj)
{
    des.readI32(obj);
}

template <>
void cubos::core::data::old::deserialize<int64_t>(Deserializer& des, int64_t& obj)
{
    des.readI64(obj);
}

template <>
void cubos::core::data::old::deserialize<uint8_t>(Deserializer& des, uint8_t& obj)
{
    des.readU8(obj);
}

template <>
void cubos::core::data::old::deserialize<uint16_t>(Deserializer& des, uint16_t& obj)
{
    des.readU16(obj);
}

template <>
void cubos::core::data::old::deserialize<uint32_t>(Deserializer& des, uint32_t& obj)
{
    des.readU32(obj);
}

template <>
void cubos::core::data::old::deserialize<uint64_t>(Deserializer& des, uint64_t& obj)
{
    des.readU64(obj);
}

template <>
void cubos::core::data::old::deserialize<float>(Deserializer& des, float& obj)
{
    des.readF32(obj);
}

template <>
void cubos::core::data::old::deserialize<double>(Deserializer& des, double& obj)
{
    des.readF64(obj);
}

template <>
void cubos::core::data::old::deserialize<bool>(Deserializer& des, bool& obj)
{
    des.readBool(obj);
}

template <>
void cubos::core::data::old::deserialize<std::string>(Deserializer& des, std::string& obj)
{
    des.readString(obj);
}

void cubos::core::data::old::deserialize(Deserializer& des, std::vector<bool>::reference obj)
{
    bool boolean;
    des.readBool(boolean);
    obj = boolean;
}

// Implementation of deserialize() for GLM types.
// A macro is used to implement for each type without repeating code.
// NOLINTBEGIN(bugprone-macro-parentheses)
#define IMPL_DESERIALIZE_GLM(T)                                                                                        \
    template <>                                                                                                        \
    void cubos::core::data::old::deserialize<glm::tvec2<T>>(Deserializer & des, glm::tvec2<T> & obj)                   \
    {                                                                                                                  \
        des.beginObject();                                                                                             \
        des.read(obj.x);                                                                                               \
        des.read(obj.y);                                                                                               \
        des.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::old::deserialize<glm::tvec3<T>>(Deserializer & des, glm::tvec3<T> & obj)                   \
    {                                                                                                                  \
        des.beginObject();                                                                                             \
        des.read(obj.x);                                                                                               \
        des.read(obj.y);                                                                                               \
        des.read(obj.z);                                                                                               \
        des.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::old::deserialize<glm::tvec4<T>>(Deserializer & des, glm::tvec4<T> & obj)                   \
    {                                                                                                                  \
        des.beginObject();                                                                                             \
        des.read(obj.x);                                                                                               \
        des.read(obj.y);                                                                                               \
        des.read(obj.z);                                                                                               \
        des.read(obj.w);                                                                                               \
        des.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::old::deserialize<glm::tquat<T>>(Deserializer & des, glm::tquat<T> & obj)                   \
    {                                                                                                                  \
        des.beginObject();                                                                                             \
        des.read(obj.w);                                                                                               \
        des.read(obj.x);                                                                                               \
        des.read(obj.y);                                                                                               \
        des.read(obj.z);                                                                                               \
        des.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::old::deserialize<glm::tmat4x4<T>>(Deserializer & des, glm::tmat4x4<T> & obj)               \
    {                                                                                                                  \
        des.beginObject();                                                                                             \
        des.read(obj[0]);                                                                                              \
        des.read(obj[1]);                                                                                              \
        des.read(obj[2]);                                                                                              \
        des.read(obj[3]);                                                                                              \
        des.endObject();                                                                                               \
    }
// NOLINTEND(bugprone-macro-parentheses)

IMPL_DESERIALIZE_GLM(int8_t)
IMPL_DESERIALIZE_GLM(int16_t)
IMPL_DESERIALIZE_GLM(int32_t)
IMPL_DESERIALIZE_GLM(int64_t)
IMPL_DESERIALIZE_GLM(uint8_t)
IMPL_DESERIALIZE_GLM(uint16_t)
IMPL_DESERIALIZE_GLM(uint32_t)
IMPL_DESERIALIZE_GLM(uint64_t)
IMPL_DESERIALIZE_GLM(float)
IMPL_DESERIALIZE_GLM(double)
IMPL_DESERIALIZE_GLM(bool)
