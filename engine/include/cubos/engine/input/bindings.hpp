/// @file
/// @brief

#pragma once

#include <cubos/core/data/serializer.hpp>

using cubos::core::data::Deserializer;
using cubos::core::data::Serializer;

namespace cubos::engine
{
    struct Bindings final
    {
        int an_integer;
    };
} // namespace cubos::engine

template <>
void cubos::core::data::serialize<cubos::engine::Bindings>(Serializer& ser, const cubos::engine::Bindings& obj,
                                                           const char* name)
{
    ser.beginObject(name);
    ser.write(obj.an_integer, "an_integer");
    ser.endObject();
}

template <>
void cubos::core::data::deserialize<cubos::engine::Bindings>(Deserializer& des, cubos::engine::Bindings& obj)
{
    des.beginObject();
    des.read(obj.an_integer);
    des.endObject();
}