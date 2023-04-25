#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cubos/core/data/deserializer.hpp>

using namespace cubos::core::data;

Deserializer::Deserializer()
{
    this->failBit = false;
}

bool Deserializer::failed() const
{
    return this->failBit;
}

void Deserializer::fail()
{
    this->failBit = true;
}

// Implementation of deserialize() for primitive types.

template <>
void cubos::core::data::deserialize<int8_t>(Deserializer& des, int8_t& val)
{
    des.readI8(val);
}

template <>
void cubos::core::data::deserialize<int16_t>(Deserializer& des, int16_t& val)
{
    des.readI16(val);
}

template <>
void cubos::core::data::deserialize<int32_t>(Deserializer& des, int32_t& val)
{
    des.readI32(val);
}

template <>
void cubos::core::data::deserialize<int64_t>(Deserializer& des, int64_t& val)
{
    des.readI64(val);
}

template <>
void cubos::core::data::deserialize<uint8_t>(Deserializer& des, uint8_t& val)
{
    des.readU8(val);
}

template <>
void cubos::core::data::deserialize<uint16_t>(Deserializer& des, uint16_t& val)
{
    des.readU16(val);
}

template <>
void cubos::core::data::deserialize<uint32_t>(Deserializer& des, uint32_t& val)
{
    des.readU32(val);
}

template <>
void cubos::core::data::deserialize<uint64_t>(Deserializer& des, uint64_t& val)
{
    des.readU64(val);
}

template <>
void cubos::core::data::deserialize<float>(Deserializer& des, float& val)
{
    des.readF32(val);
}

template <>
void cubos::core::data::deserialize<double>(Deserializer& des, double& val)
{
    des.readF64(val);
}

template <>
void cubos::core::data::deserialize<bool>(Deserializer& des, bool& val)
{
    des.readBool(val);
}

template <>
void cubos::core::data::deserialize<std::string>(Deserializer& des, std::string& val)
{
    des.readString(val);
}

void cubos::core::data::deserialize(Deserializer& des, std::vector<bool>::reference val)
{
    bool boolean;
    des.readBool(boolean);
    val = boolean;
}

// Implementation of deserialize() for GLM types.
// A macro is used to implement for each type without repeating code.
// NOLINTBEGIN(bugprone-macro-parentheses)
#define IMPL_DESERIALIZE_GLM(T)                                                                                        \
    template <>                                                                                                        \
    void cubos::core::data::deserialize<glm::tvec2<T>>(Deserializer & des, glm::tvec2<T> & val)                        \
    {                                                                                                                  \
        des.beginObject();                                                                                             \
        des.read(val.x);                                                                                               \
        des.read(val.y);                                                                                               \
        des.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::deserialize<glm::tvec3<T>>(Deserializer & des, glm::tvec3<T> & val)                        \
    {                                                                                                                  \
        des.beginObject();                                                                                             \
        des.read(val.x);                                                                                               \
        des.read(val.y);                                                                                               \
        des.read(val.z);                                                                                               \
        des.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::deserialize<glm::tvec4<T>>(Deserializer & des, glm::tvec4<T> & val)                        \
    {                                                                                                                  \
        des.beginObject();                                                                                             \
        des.read(val.x);                                                                                               \
        des.read(val.y);                                                                                               \
        des.read(val.z);                                                                                               \
        des.read(val.w);                                                                                               \
        des.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::deserialize<glm::tquat<T>>(Deserializer & des, glm::tquat<T> & val)                        \
    {                                                                                                                  \
        des.beginObject();                                                                                             \
        des.read(val.w);                                                                                               \
        des.read(val.x);                                                                                               \
        des.read(val.y);                                                                                               \
        des.read(val.z);                                                                                               \
        des.endObject();                                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    template <>                                                                                                        \
    void cubos::core::data::deserialize<glm::tmat4x4<T>>(Deserializer & des, glm::tmat4x4<T> & val)                    \
    {                                                                                                                  \
        des.beginObject();                                                                                             \
        des.read(val[0]);                                                                                              \
        des.read(val[1]);                                                                                              \
        des.read(val[2]);                                                                                              \
        des.read(val[3]);                                                                                              \
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
