#include <cubos/gl/grid.hpp>
#include <cubos/log.hpp>

using namespace cubos::gl;

Grid::Grid(const glm::ivec3& size, Grid::IndexWidth width)
{
    if (size.x < 1 || size.y < 1 || size.z < 1)
    {
        logWarning("Grid size must be at least 1 in each dimension: was ({}, {}, {}), defaulting to (1, 1, 1).", size.x,
                   size.y, size.z);
        this->size = {1, 1, 1};
    }
    else
        this->size = size;

    this->width = width;
    if (this->width == IndexWidth::U8)
        new (&this->shortIndices) std::vector<uint8_t>(this->size.x * this->size.y * this->size.z, 0);
    else
        new (&this->longIndices) std::vector<uint16_t>(this->size.x * this->size.y * this->size.z, 0);
}

Grid::Grid(const glm::ivec3& size, const std::vector<uint8_t>& indices)
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

    this->width = IndexWidth::U8;
    new (&this->shortIndices) std::vector<uint8_t>(indices);
}

Grid::Grid(const glm::ivec3& size, const std::vector<uint16_t>& indices)
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

    this->width = IndexWidth::U16;
    new (&this->longIndices) std::vector<uint16_t>(indices);
}

Grid::Grid() : width(IndexWidth::U8)
{
    this->size = {1, 1, 1};
    new (&this->shortIndices) std::vector<uint8_t>(1, 0);
}

Grid::~Grid()
{
    if (this->width == IndexWidth::U8)
        this->shortIndices.~vector();
    else
        this->longIndices.~vector();
}

void Grid::setIndexWidth(IndexWidth width)
{
    if (width == this->width)
        return;
    this->width = width;

    if (this->width == IndexWidth::U8)
    {
        this->longIndices.~vector();
        new (&this->shortIndices) std::vector<uint8_t>(this->size.x * this->size.y * this->size.z, 0);
    }
    else
    {
        this->shortIndices.~vector();
        new (&this->longIndices) std::vector<uint16_t>(this->size.x * this->size.y * this->size.z, 0);
    }
}

Grid::IndexWidth Grid::getIndexWidth() const
{
    return this->width;
}

void Grid::setSize(const glm::ivec3& size)
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

    if (this->width == IndexWidth::U8)
        this->shortIndices.resize(this->size.x * this->size.y * this->size.z, 0);
    else
        this->longIndices.resize(this->size.x * this->size.y * this->size.z, 0);
}

size_t Grid::get(const glm::ivec3& position) const
{
    if (position.x < 0 || position.x >= size.x || position.y < 0 || position.y >= size.y || position.z < 0 ||
        position.z >= size.z)
        return 0;
    if (this->width == IndexWidth::U8)
        return this->shortIndices[position.x + position.y * size.x + position.z * size.x * size.y];
    else
        return this->longIndices[position.x + position.y * size.x + position.z * size.x * size.y];
}

void Grid::set(const glm::ivec3& position, size_t mat)
{
    if (position.x < 0 || position.x >= size.x || position.y < 0 || position.y >= size.y || position.z < 0 ||
        position.z >= size.z)
        return;
    if (this->width == IndexWidth::U8)
        this->shortIndices[position.x + position.y * size.x + position.z * size.x * size.y] = mat;
    else
        this->longIndices[position.x + position.y * size.x + position.z * size.x * size.y] = mat;
}

void Grid::serialize(memory::Serializer& serializer) const
{
    serializer.write(this->size, "size");
    if (this->width == IndexWidth::U8)
    {
        serializer.write(static_cast<int8_t>(8), "indexWidth");
        serializer.write(this->shortIndices, "data");
    }
    else
    {
        serializer.write(static_cast<int8_t>(16), "indexWidth");
        serializer.write(this->longIndices, "data");
    }
}

void Grid::deserialize(memory::Deserializer& deserializer)
{
    glm::ivec3 size;
    int8_t width;
    deserializer.read(size);
    deserializer.read(width);
    if (width == 8)
    {
        this->setIndexWidth(IndexWidth::U8);
        deserializer.read(this->shortIndices);
    }
    else if (width == 16)
    {
        this->setIndexWidth(IndexWidth::U16);
        deserializer.read(this->longIndices);
    }
    else
    {
        logWarning("Could not deserialize grid: index width {} is not supported.", width);
        return;
    }

    this->size = size;
}
