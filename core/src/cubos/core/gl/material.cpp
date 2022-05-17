#include <cubos/core/gl/material.hpp>

using namespace cubos::core::gl;

const Material Material::Empty = {{0, 0, 0, 0}};

/// Returns the similarity between two colors.
static float colorSimilarity(const glm::vec4& a, const glm::vec4& b)
{
    return 1.0f - glm::abs(a.r - b.r) * glm::abs(a.g - b.g) * glm::abs(a.b - b.b) * glm::abs(a.a - b.a);
}

float Material::similarity(const Material& other) const
{
    return colorSimilarity(this->color, other.color);
}

void cubos::core::data::serialize(Serializer& serializer, const Material& mat, const char* name)
{
    serializer.write(mat.color, name);
}

void cubos::core::data::deserialize(Deserializer& deserializer, Material& mat)
{
    deserializer.read(mat.color);
}
