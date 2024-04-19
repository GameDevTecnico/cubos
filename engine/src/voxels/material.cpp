#include <cubos/engine/voxels/material.hpp>

using namespace cubos::engine;

const VoxelMaterial VoxelMaterial::Empty = {{0, 0, 0, 0}};

/// Returns the similarity between two colors.
static float colorSimilarity(const glm::vec4& a, const glm::vec4& b)
{
    return 1.0F - (glm::abs(a.r - b.r) + glm::abs(a.g - b.g) + glm::abs(a.b - b.b) + glm::abs(a.a - b.a)) / 4.0F;
}

float VoxelMaterial::similarity(const VoxelMaterial& other) const
{
    return colorSimilarity(this->color, other.color);
}
