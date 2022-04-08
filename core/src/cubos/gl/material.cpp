#include <cubos/gl/material.hpp>

using namespace cubos::core::gl;

const Material Material::Empty = {{0, 0, 0, 0}};

void Material::serialize(memory::Serializer& serializer) const
{
    serializer.write(color, "color");
}

void Material::deserialize(memory::Deserializer& deserializer)
{
    deserializer.read(color);
}
