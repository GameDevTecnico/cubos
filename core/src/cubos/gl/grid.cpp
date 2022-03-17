#include <cubos/gl/grid.hpp>
#include <cubos/log.hpp>

using namespace cubos::gl;

Grid::Grid(const glm::uvec3& size)
{
    if (size.x < 1 || size.y < 1 || size.z < 1)
    {
        logWarning("Grid size must be at least 1 in each dimension: was ({}, {}, {}), defaulting to (1, 1, 1).", size.x,
                   size.y, size.z);
        this->size = {1, 1, 1};
    }
    else
        this->size = size;
    this->indices.resize(this->size.x * this->size.y * this->size.z, 0);
}

Grid::Grid(const glm::uvec3& size, const std::vector<uint16_t>& indices)
{
    if (size.x < 1 || size.y < 1 || size.z < 1)
    {
        logWarning("Grid size must be at least 1 in each dimension: was ({}, {}, {}), defaulting to (1, 1, 1).", size.x,
                   size.y, size.z);
        this->size = {1, 1, 1};
    }
    else if (indices.size() != size.x * size.y * size.z)
    {
        logWarning("Grid size and indices size mismatch: was ({}, {}, {}), indices size is {}.", size.x, size.y, size.z,
                   indices.size());
        this->size = {1, 1, 1};
    }
    else
        this->size = size;
    this->indices = indices;
}

Grid::Grid(Grid&& other) : size(other.size)
{
    new (&this->indices) std::vector<uint16_t>(std::move(other.indices));
}

Grid::Grid()
{
    this->size = {1, 1, 1};
    this->indices.resize(1, 0);
}

void Grid::setSize(const glm::uvec3& size)
{
    if (size == this->size)
        return;
    else if (size.x < 1 || size.y < 1 || size.z < 1)
    {
        logWarning("Grid size must be at least 1 in each dimension: preserving original dimensions (tried to set to "
                   "({}, {}, {}))",
                   size.x, size.y, size.z);
        return;
    }

    this->size = size;
    this->indices.clear();
    this->indices.resize(this->size.x * this->size.y * this->size.z, 0);
}

const glm::uvec3& Grid::getSize() const
{
    return size;
}

void Grid::clear()
{
    for (size_t i = 0; i < this->indices.size(); i++)
        this->indices[i] = 0;
}

uint16_t Grid::get(const glm::ivec3& position) const
{
    assert(position.x >= 0 && position.x < this->size.x && position.y >= 0 && position.y < this->size.y &&
           position.z >= 0 && position.z < this->size.z);
    return this->indices[position.x + position.y * size.x + position.z * size.x * size.y];
}

void Grid::set(const glm::ivec3& position, uint16_t mat)
{
    assert(position.x >= 0 && position.x < this->size.x && position.y >= 0 && position.y < this->size.y &&
           position.z >= 0 && position.z < this->size.z);
    this->indices[position.x + position.y * size.x + position.z * size.x * size.y] = mat;
}

void Grid::serialize(memory::Serializer& serializer) const
{
    serializer.write(this->size, "size");
    serializer.write(this->indices, "data");
}

void Grid::deserialize(memory::Deserializer& deserializer)
{
    deserializer.read(this->size);
    deserializer.read(this->indices);

    if (this->size.x * this->size.y * this->size.z != static_cast<int>(this->indices.size()))
    {
        logWarning(
            "Could not deserialize grid: grid size and indices size mismatch: was ({}, {}, {}), indices size is {}.",
            size.x, size.y, size.z, indices.size());
        this->size = {1, 1, 1};
        this->indices.clear();
        this->indices.resize(1, 0);
    }
}
