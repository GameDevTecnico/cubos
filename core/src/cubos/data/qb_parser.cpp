#include <cubos/data/qb_parser.hpp>
#include <cubos/memory/endianness.hpp>

bool cubos::data::parseQB(std::vector<QBMatrix>& matrices, memory::Stream& stream)
{
    uint8_t version[4] = {0, 0, 0, 0};
    uint32_t colorFormat, zAxisOrientation, compressed, visibilityMaskEncoded, numMatrices;

    // Parse the version of the file.
    stream.read(version, 4);
    if (stream.eof())
    {
        logError("parseQB(): unexpected end of file while reading file version");
        return false;
    }
    else if (version[0] != 1 || version[1] != 1 || version[2] != 0 || version[3] != 0)
    {
        logError("parseQB(): unsupported version {}.{}.{}.{}, only 1.1.0.0 is supported", version[0], version[1],
                 version[2], version[3]);
        return false;
    }

    // Parse the rest of the header.
    stream.read(&colorFormat, 4);
    stream.read(&zAxisOrientation, 4);
    stream.read(&compressed, 4);
    stream.read(&visibilityMaskEncoded, 4);
    stream.read(&numMatrices, 4);
    colorFormat = memory::fromBigEndian(colorFormat);
    zAxisOrientation = memory::fromBigEndian(zAxisOrientation);
    compressed = memory::fromBigEndian(compressed);
    visibilityMaskEncoded = memory::fromBigEndian(visibilityMaskEncoded);
    numMatrices = memory::fromBigEndian(numMatrices);

    if (stream.eof())
    {
        logError("parseQB(): unexpected end of file while reading file header");
        return false;
    }
    else if (visibilityMaskEncoded)
    {
        logError("parseQB(): visibility mask encoding is not supported");
        return false;
    }

    // Parse the matrices.
    matrices.resize(numMatrices);
    for (uint32_t i = 0; i < numMatrices; ++i)
    {
        uint8_t nameLen;
        uint32_t sizeX, sizeY, sizeZ;
        int32_t posX, posY, posZ;

        // Read the matrix name.
        stream.read(&nameLen, 1);
        std::string name(nameLen, ' ');
        stream.read(name.data(), nameLen);

        // Read the matrix size.
        stream.read(&sizeX, 4);
        stream.read(&sizeY, 4);
        stream.read(&sizeZ, 4);
        sizeX = memory::fromBigEndian(sizeX);
        sizeY = memory::fromBigEndian(sizeY);
        sizeZ = memory::fromBigEndian(sizeZ);
        matrices[i].grid.setSize({sizeX, sizeY, sizeZ});

        // Read the matrix position.
        stream.read(&posX, 4);
        stream.read(&posY, 4);
        stream.read(&posZ, 4);
        posX = memory::fromBigEndian(posX);
        posY = memory::fromBigEndian(posY);
        posZ = memory::fromBigEndian(posZ);
        matrices[i].position = glm::ivec3(posX, posY, posZ);

        // Read the matrix voxels.
        if (compressed == 0)
        {
            uint8_t color[4];
            size_t nextMat = 1;

            for (uint32_t z = 0; z < sizeZ; ++z)
                for (uint32_t y = 0; y < sizeY; ++y)
                    for (uint32_t x = 0; x < sizeX; ++x)
                    {
                        stream.read(color, 4);
                        if (color[3] == 0)
                            continue;
                        else if (colorFormat) // BGRA -> RGBA
                            std::swap(color[0], color[2]);
                        glm::vec4 colorVec(color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f, color[3] / 255.0f);

                        // Check if the material is already in the palette.
                        size_t mat;
                        for (mat = 1; mat < nextMat; ++mat)
                            if (matrices[i].palette.get(mat).color == colorVec)
                                break;
                        if (mat == nextMat)
                        {
                            // Add the material to the palette.
                            gl::Material desc;
                            desc.color = colorVec;
                            matrices[i].palette.set(mat, desc);
                            nextMat += 1;

                            if (mat >= 65536)
                            {
                                logError("parseQB(): too many materials, max is 65536");
                                return false;
                            }
                        }

                        // Set the voxel.
                        matrices[i].grid.set(glm::ivec3(x, y, z), mat);
                    }
        }
        else
        {
            logError("parseQB(): compressed QB files are not supported");
            return false;
        }
    }

    return true;
}
