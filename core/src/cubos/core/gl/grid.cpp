#include <algorithm>
#include <unordered_map>

#include <cubos/core/gl/grid.hpp>
#include <cubos/core/gl/palette.hpp>
#include <cubos/core/log.hpp>

using namespace cubos::core::gl;

Grid::Grid(const glm::uvec3& size)
{
    if (size.x < 1 || size.y < 1 || size.z < 1)
    {
        CUBOS_WARN("Grid size must be at least 1 in each dimension: was ({}, {}, {}), defaulting to (1, 1, 1).", size.x,
                   size.y, size.z);
        this->size = {1, 1, 1};
    }
    else
    {
        this->size = size;
    }
    this->indices.resize(
        static_cast<size_t>(this->size.x) * static_cast<size_t>(this->size.y) * static_cast<size_t>(this->size.z), 0);
}

Grid::Grid(const glm::uvec3& size, const std::vector<uint16_t>& indices)
{
    if (size.x < 1 || size.y < 1 || size.z < 1)
    {
        CUBOS_WARN("Grid size must be at least 1 in each dimension: was ({}, {}, {}), defaulting to (1, 1, 1).", size.x,
                   size.y, size.z);
        this->size = {1, 1, 1};
    }
    else if (indices.size() != static_cast<size_t>(size.x) * static_cast<size_t>(size.y) * static_cast<size_t>(size.z))
    {
        CUBOS_WARN("Grid size and indices size mismatch: was ({}, {}, {}), indices size is {}.", size.x, size.y, size.z,
                   indices.size());
        this->size = {1, 1, 1};
    }
    else
    {
        this->size = size;
    }
    this->indices = indices;
}

Grid::Grid(Grid&& other)
    : size(other.size)
{
    new (&this->indices) std::vector<uint16_t>(std::move(other.indices));
}

Grid::Grid()
{
    this->size = {1, 1, 1};
    this->indices.resize(1, 0);
}

Grid& Grid::operator=(const Grid& rhs) = default;

void Grid::setSize(const glm::uvec3& size)
{
    if (size == this->size)
    {
        return;
    }
    if (size.x < 1 || size.y < 1 || size.z < 1)
    {
        CUBOS_WARN("Grid size must be at least 1 in each dimension: preserving original dimensions (tried to set to "
                   "({}, {}, {}))",
                   size.x, size.y, size.z);
        return;
    }

    this->size = size;
    this->indices.clear();
    this->indices.resize(
        static_cast<size_t>(this->size.x) * static_cast<size_t>(this->size.y) * static_cast<size_t>(this->size.z), 0);
}

const glm::uvec3& Grid::getSize() const
{
    return size;
}

void Grid::clear()
{
    std::ranges::fill(this->indices, 0);
}

uint16_t Grid::get(const glm::ivec3& position) const
{
    assert(position.x >= 0 && position.x < static_cast<int>(this->size.x));
    assert(position.y >= 0 && position.y < static_cast<int>(this->size.y));
    assert(position.z >= 0 && position.z < static_cast<int>(this->size.z));
    auto index = position.x + position.y * static_cast<int>(size.x) + position.z * static_cast<int>(size.x * size.y);
    return this->indices[static_cast<size_t>(index)];
}

void Grid::set(const glm::ivec3& position, uint16_t mat)
{
    assert(position.x >= 0 && position.x < static_cast<int>(this->size.x));
    assert(position.y >= 0 && position.y < static_cast<int>(this->size.y));
    assert(position.z >= 0 && position.z < static_cast<int>(this->size.z));
    auto index = position.x + position.y * static_cast<int>(size.x) + position.z * static_cast<int>(size.x * size.y);
    this->indices[static_cast<size_t>(index)] = mat;
}

bool Grid::convert(const Palette& src, const Palette& dst, float minSimilarity)
{
    // Find the mappings for every material in the source palette.
    std::unordered_map<uint16_t, uint16_t> mappings;
    for (uint16_t i = 0; i <= src.getSize(); ++i)
    {
        uint16_t j = dst.find(src.get(i));
        if (src.get(i).similarity(dst.get(j)) >= minSimilarity)
        {
            mappings[i] = j;
        }
    }

    // Check if the mappings are complete for every material being used in the grid.
    for (uint32_t i = 0; i < this->size.x * this->size.y * this->size.z; ++i)
    {
        if (mappings.find(this->indices[i]) == mappings.end())
        {
            return false;
        }
    }

    // Apply the mappings.
    for (uint32_t i = 0; i < this->size.x * this->size.y * this->size.z; ++i)
    {
        this->indices[i] = mappings[this->indices[i]];
    }

    return true;
}

void cubos::core::data::serialize(Serializer& serializer, const gl::Grid& grid, const char* name)
{
    serializer.beginObject(name);
    serializer.write(grid.size, "size");
    serializer.write(grid.indices, "data");
    serializer.endObject();
}

void cubos::core::data::deserialize(Deserializer& deserializer, gl::Grid& grid)
{
    deserializer.beginObject();
    deserializer.read(grid.size);
    deserializer.read(grid.indices);
    deserializer.endObject();

    if (grid.size.x * grid.size.y * grid.size.z != static_cast<unsigned int>(grid.indices.size()))
    {
        CUBOS_WARN("Grid size and indices size mismatch: was ({}, {}, {}), indices size is {}.", grid.size.x,
                   grid.size.y, grid.size.z, grid.indices.size());
        grid.size = {1, 1, 1};
        grid.indices.clear();
        grid.indices.resize(1, 0);
    }
}
