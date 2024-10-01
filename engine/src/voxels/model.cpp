#include <cstring>

#include <cubos/core/log.hpp>
#include <cubos/core/memory/endianness.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/traits/constructible.hpp>
#include <cubos/core/reflection/type.hpp>

#include <cubos/engine/voxels/model.hpp>

namespace memory = cubos::core::memory;

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(VoxelModel)
{
    using namespace cubos::core::reflection;

    return Type::create("cubos::engine::VoxelModel")
        .with(ConstructibleTrait::typed<VoxelModel>().withDefaultConstructor().withMoveConstructor().build());
}

std::size_t VoxelModel::gridCount() const
{
    return mMatrices.size();
}

const VoxelPalette& VoxelModel::palette() const
{
    return mPalette;
}

VoxelPalette& VoxelModel::palette()
{
    return mPalette;
}

const VoxelGrid& VoxelModel::grid(std::size_t index) const
{
    return mMatrices[index].grid;
}

VoxelGrid& VoxelModel::grid(std::size_t index)
{
    return mMatrices[index].grid;
}

glm::ivec3 VoxelModel::gridPosition(std::size_t index) const
{
    return mMatrices[index].position;
}

void VoxelModel::gridPosition(std::size_t index, glm::ivec3 position)
{
    mMatrices[index].position = position;
}

bool VoxelModel::loadFrom(memory::Stream& stream)
{
    uint8_t version[4] = {0, 0, 0, 0};
    uint32_t colorFormat;
    uint32_t zAxisOrientation;
    uint32_t compressed;
    uint32_t visibilityMaskEncoded;
    uint32_t numMatrices;

    // Parse the version of the file.
    stream.read(version, 4);
    if (stream.eof())
    {
        CUBOS_ERROR("Unexpected end of file while reading file version");
        return false;
    }
    if (version[0] != 1 || version[1] != 1 || version[2] != 0 || version[3] != 0)
    {
        CUBOS_ERROR("Unsupported version {}.{}.{}.{}, only 1.1.0.0 is supported", version[0], version[1], version[2],
                    version[3]);
        return false;
    }

    // Parse the rest of the header.
    stream.read(&colorFormat, 4);
    stream.read(&zAxisOrientation, 4);
    stream.read(&compressed, 4);
    stream.read(&visibilityMaskEncoded, 4);
    stream.read(&numMatrices, 4);
    colorFormat = memory::fromLittleEndian(colorFormat);
    zAxisOrientation = memory::fromLittleEndian(zAxisOrientation);
    compressed = memory::fromLittleEndian(compressed);
    visibilityMaskEncoded = memory::fromLittleEndian(visibilityMaskEncoded);
    numMatrices = memory::fromLittleEndian(numMatrices);

    if (stream.eof())
    {
        CUBOS_ERROR("Unexpected end of file while reading file header");
        return false;
    }
    if (visibilityMaskEncoded != 0U)
    {
        CUBOS_ERROR("Visibility mask encoding is not supported");
        return false;
    }

    // Parse the matrices.
    mMatrices.resize(numMatrices);
    for (uint32_t i = 0; i < numMatrices; ++i)
    {
        uint8_t nameLen;
        uint32_t sizeX;
        uint32_t sizeY;
        uint32_t sizeZ;
        int32_t posX;
        int32_t posY;
        int32_t posZ;

        // Read the matrix name.
        stream.read(&nameLen, 1);
        std::string name(nameLen, ' ');
        stream.read(name.data(), nameLen);

        // Read the matrix size.
        stream.read(&sizeX, 4);
        stream.read(&sizeY, 4);
        stream.read(&sizeZ, 4);
        sizeX = memory::fromLittleEndian(sizeX);
        sizeY = memory::fromLittleEndian(sizeY);
        sizeZ = memory::fromLittleEndian(sizeZ);
        mMatrices[i].grid.setSize({sizeX, sizeY, sizeZ});

        // Read the matrix position.
        stream.read(&posX, 4);
        stream.read(&posY, 4);
        stream.read(&posZ, 4);
        posX = memory::fromLittleEndian(posX);
        posY = memory::fromLittleEndian(posY);
        posZ = memory::fromLittleEndian(posZ);
        mMatrices[i].position = glm::ivec3(posX, posY, posZ);

        // Read the matrix voxels.
        if (compressed == 0)
        {
            uint8_t color[4];
            std::size_t nextMat = 1;

            for (uint32_t z = 0; z < sizeZ; ++z)
            {
                for (uint32_t y = 0; y < sizeY; ++y)
                {
                    for (uint32_t x = 0; x < sizeX; ++x)
                    {
                        stream.read(color, 4);
                        if (color[3] == 0)
                        {
                            continue;
                        }
                        if (colorFormat != 0U)
                        { // BGRA -> RGBA
                            std::swap(color[0], color[2]);
                        }
                        glm::vec4 colorVec(static_cast<float>(color[0]) / 255.0F, static_cast<float>(color[1]) / 255.0F,
                                           static_cast<float>(color[2]) / 255.0F,
                                           static_cast<float>(color[3]) / 255.0F);

                        // Check if the material is already in the palette.
                        std::size_t mat;
                        for (mat = 1; mat < nextMat; ++mat)
                        {
                            if (mPalette.get(static_cast<uint16_t>(mat)).color == colorVec)
                            {
                                break;
                            }
                        }
                        if (mat == nextMat)
                        {
                            if (mat >= VoxelPalette::MaxMaterials)
                            {
                                CUBOS_ERROR("Too many materials, max is {}", VoxelPalette::MaxMaterials);
                                return false;
                            }

                            // Add the material to the palette.
                            VoxelMaterial desc;
                            desc.color = colorVec;
                            mPalette.set(static_cast<uint16_t>(mat), desc);
                            nextMat += 1;
                        }

                        // Set the voxel.
                        mMatrices[i].grid.set(glm::ivec3(x, y, z), static_cast<uint16_t>(mat));
                    }
                }
            }
        }
        else
        {
            CUBOS_ERROR("Compressed QB files are not supported");
            return false;
        }
    }

    return true;
}

bool VoxelModel::writeTo(memory::Stream& stream) const
{
    // Write the version of the file.
    uint8_t version[4] = {1, 1, 0, 0};
    stream.write(version, 4);

    // Write header information.
    uint32_t colorFormat = 0;           // Assuming 0 for RGBA
    uint32_t zAxisOrientation = 0;      // Assuming 0
    uint32_t compressed = 0;            // No compression
    uint32_t visibilityMaskEncoded = 0; // No visibility mask
    auto numMatrices = static_cast<uint32_t>(mMatrices.size());

    stream.write(&colorFormat, 4);
    stream.write(&zAxisOrientation, 4);
    stream.write(&compressed, 4);
    stream.write(&visibilityMaskEncoded, 4);
    stream.write(&numMatrices, 4);

    // Write each matrix.
    for (const auto& matrix : mMatrices)
    {
        const auto& grid = matrix.grid;

        // Write the matrix name.
        std::string name = "matrix"; // Assuming a default name or you may have a name in the matrix structure
        auto nameLen = static_cast<uint8_t>(name.size());
        stream.write(&nameLen, 1);
        stream.write(name.data(), nameLen);

        // Write the matrix size.
        auto size = grid.size();
        uint32_t sizeX = size.x;
        uint32_t sizeY = size.y;
        uint32_t sizeZ = size.z;

        sizeX = memory::toLittleEndian(sizeX);
        sizeY = memory::toLittleEndian(sizeY);
        sizeZ = memory::toLittleEndian(sizeZ);

        stream.write(&sizeX, 4);
        stream.write(&sizeY, 4);
        stream.write(&sizeZ, 4);

        // Write the matrix position.
        auto pos = matrix.position;
        int32_t posX = pos.x;
        int32_t posY = pos.y;
        int32_t posZ = pos.z;

        posX = memory::toLittleEndian(posX);
        posY = memory::toLittleEndian(posY);
        posZ = memory::toLittleEndian(posZ);

        stream.write(&posX, 4);
        stream.write(&posY, 4);
        stream.write(&posZ, 4);

        // Write the matrix voxels.
        uint8_t color[4];
        for (uint32_t z = 0; z < sizeZ; ++z)
        {
            for (uint32_t y = 0; y < sizeY; ++y)
            {
                for (uint32_t x = 0; x < sizeX; ++x)
                {
                    auto voxel = grid.get(glm::ivec3(x, y, z));
                    if (voxel == 0)
                    {
                        // Write a transparent voxel.
                        color[0] = 0;
                        color[1] = 0;
                        color[2] = 0;
                        color[3] = 0;
                    }
                    else
                    {
                        // Write the color of the voxel.
                        auto matDesc = mPalette.get(voxel);
                        auto colorVec = matDesc.color;

                        color[0] = static_cast<uint8_t>(colorVec.r * 255.0F);
                        color[1] = static_cast<uint8_t>(colorVec.g * 255.0F);
                        color[2] = static_cast<uint8_t>(colorVec.b * 255.0F);
                        color[3] = static_cast<uint8_t>(colorVec.a * 255.0F);

                        if (colorFormat != 0U) // Convert to BGRA if needed
                        {
                            std::swap(color[0], color[2]);
                        }
                    }
                    stream.write(color, 4);
                }
            }
        }
    }

    return true;
}