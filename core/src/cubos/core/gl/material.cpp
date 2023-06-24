#include <cubos/core/gl/material.hpp>

using namespace cubos::core::gl;

const Material Material::Empty = {{0, 0, 0, 0}};

/// Returns the similarity between two colors.
static float colorSimilarity(const glm::vec4& a, const glm::vec4& b)
{
    return 1.0F - (glm::abs(a.r - b.r) + glm::abs(a.g - b.g) + glm::abs(a.b - b.b) + glm::abs(a.a - b.a)) / 4.0F;
}

float Material::similarity(const Material& other) const
{
    return colorSimilarity(this->color, other.color);
}

void cubos::core::old::data::serialize(Serializer& serializer, const Material& mat, const char* name)
{
    serializer.beginObject(name);
    serializer.write(mat.color.r, "r");
    serializer.write(mat.color.g, "g");
    serializer.write(mat.color.b, "b");
    serializer.write(mat.color.a, "a");
    serializer.endObject();
}

void cubos::core::old::data::deserialize(Deserializer& deserializer, Material& mat)
{
    deserializer.beginObject();
    deserializer.read(mat.color.r);
    deserializer.read(mat.color.g);
    deserializer.read(mat.color.b);
    deserializer.read(mat.color.a);
    deserializer.endObject();
}
