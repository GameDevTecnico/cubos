#include <cubos/core/gl/material.hpp>

using namespace cubos::core::gl;

const Material Material::Empty = {{0, 0, 0, 0}};

void cubos::core::data::serialize(Serializer& serializer, const Material& mat, const char* name)
{
    serializer.write(mat.color, name);
}

void cubos::core::data::deserialize(Deserializer& deserializer, Material& mat)
{
    deserializer.read(mat.color);
}
