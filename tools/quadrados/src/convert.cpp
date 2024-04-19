#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include <cubos/core/log.hpp>
#include <cubos/core/memory/endianness.hpp>
#include <cubos/core/memory/standard_stream.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/voxels/palette.hpp>

#include "tools.hpp"

namespace memory = cubos::core::memory;
using namespace cubos::engine;

namespace fs = std::filesystem;

/// The input options of the program.
struct ConvertOptions
{
    fs::path input = "";                             ///< The input file path.
    fs::path palette = "";                           ///< The palette path.
    std::unordered_map<std::size_t, fs::path> grids; ///< The output paths of the grids.
    bool write = false;                              ///< Whether to write to the palette.
    bool verbose = false;                            ///< Enables verbose mode.
    bool force = false;                              ///< Enables force mode.
    bool help = false;                               ///< Prints the help message.
    float similarity = 1.0F;                         ///< The similarity threshold.
};

/// @brief Represents the data read from a matrix in a QB file.
struct QBMatrix
{
    VoxelGrid grid;       ///< Grid of the matrix.
    VoxelPalette palette; ///< VoxelPalette of the matrix.
    glm::ivec3 position;  ///< Position of the matrix.
};

/// Prints the help message of the program.
static void printHelp()
{
    std::cerr << "Usage: quadrados convert <INPUT> -p <PALETTE-PATH> [OPTIONS]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -g<N> <PATH> Sets the output path of the grid <N>." << std::endl;
    std::cerr << "  -p <PATH>    Specifies the path of the palette being used." << std::endl;
    std::cerr << "  -w           Allows the palette to be written to." << std::endl;
    std::cerr << "  -v           Enables verbose mode." << std::endl;
    std::cerr << "  -f           Disables asking for confirmation when overwriting files." << std::endl;
    std::cerr << "  -h           Prints this help message." << std::endl;
    std::cerr << "  -s <VAL>     Specifies the minimum similarity for two materials to be merged," << std::endl;
    std::cerr << "               from 0.0 to 1.0 (default 1.0)" << std::endl;
}

/// Parses the command line arguments.
/// @param argc The number of arguments.
/// @param argv The arguments.
/// @param options The options to fill.
/// @return True if the arguments were parsed successfully, false otherwise.
static bool parseArguments(int argc, char** argv, ConvertOptions& options)
{
    bool foundInput = false;

    // Iterate over the arguments.
    for (int i = 0; i < argc; ++i)
    {
        if (std::string(argv[i]).substr(0, 2) == "-g")
        {
            int index = argv[i][2] == '\0' ? 0 : std::stoi(std::string(argv[i]).substr(2));

            if (i + 1 < argc)
            {
                if (options.grids.find(static_cast<std::size_t>(index)) == options.grids.end())
                {
                    options.grids[static_cast<std::size_t>(index)] = argv[i + 1];
                }
                else
                {
                    std::cerr << "Output path for grid " << index << " already specified." << std::endl;
                    return false;
                }

                i++;
            }
            else
            {
                std::cerr << "Missing argument for " << argv[i] << "." << std::endl;
                return false;
            }
        }
        else if (std::string(argv[i]) == "-p")
        {
            if (i + 1 < argc)
            {
                options.palette = argv[i + 1];
                i++;
            }
            else
            {
                std::cerr << "Missing argument for -p." << std::endl;
                return false;
            }
        }
        else if (std::string(argv[i]) == "-s")
        {
            if (i + 1 < argc)
            {
                options.similarity = std::stof(argv[i + 1]);
                if (options.similarity < 0.0F || options.similarity > 1.0F)
                {
                    std::cerr << "Invalid similarity value " << options.similarity << "." << std::endl;
                    return false;
                }

                i++;
            }
            else
            {
                std::cerr << "Missing argument for -s." << std::endl;
                return false;
            }
        }
        else if (std::string(argv[i]) == "-w")
        {
            options.write = true;
        }
        else if (std::string(argv[i]) == "-v")
        {
            options.verbose = true;
        }
        else if (std::string(argv[i]) == "-f")
        {
            options.force = true;
        }
        else if (std::string(argv[i]) == "-h")
        {
            options.help = true;
            return true;
        }
        else
        {
            if (foundInput)
            {
                std::cerr << "Too many arguments." << std::endl;
                return false;
            }

            foundInput = true;
            options.input = argv[i];
        }
    }

    if (options.input.empty())
    {
        std::cerr << "Missing input file." << std::endl;
        return false;
    }
    if (options.palette.empty() && !options.verbose)
    {
        std::cerr << "Missing input palette." << std::endl;
        return false;
    }
    return true;
}

/// @brief Parses a Qubicle file (.qb), pushing every matrix found in the file to the passed vector.
/// @param[out] matrices Parsed matrices.
/// @param stream Stream to read from.
/// @return Whether the file was parsed successfully.
bool parseQB(std::vector<QBMatrix>& matrices, memory::Stream& stream)
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
    matrices.resize(numMatrices);
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
        matrices[i].grid.setSize({sizeX, sizeY, sizeZ});

        // Read the matrix position.
        stream.read(&posX, 4);
        stream.read(&posY, 4);
        stream.read(&posZ, 4);
        posX = memory::fromLittleEndian(posX);
        posY = memory::fromLittleEndian(posY);
        posZ = memory::fromLittleEndian(posZ);
        matrices[i].position = glm::ivec3(posX, posY, posZ);

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
                            if (matrices[i].palette.get(static_cast<uint16_t>(mat)).color == colorVec)
                            {
                                break;
                            }
                        }
                        if (mat == nextMat)
                        {
                            if (mat >= 65536)
                            {
                                CUBOS_ERROR("Too many materials, max is 65536");
                                return false;
                            }

                            // Add the material to the palette.
                            VoxelMaterial desc;
                            desc.color = colorVec;
                            matrices[i].palette.set(static_cast<uint16_t>(mat), desc);
                            nextMat += 1;
                        }

                        // Set the voxel.
                        matrices[i].grid.set(glm::ivec3(x, y, z), static_cast<uint16_t>(mat));
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

/// Tries to load the palette from the given path.
/// @param path The path of the palette.
/// @param palette The palette to fill.
static bool loadPalette(const fs::path& path, VoxelPalette& palette)
{
    auto* file = fopen(path.string().c_str(), "rb");
    if (file == nullptr)
    {
        return false;
    }

    auto stream = memory::StandardStream(file, true);
    if (!palette.loadFrom(stream))
    {
        std::cerr << "Failed to deserialize palette." << std::endl;
        return false;
    }

    return true;
}

/// Tries to load a QBModel from the given path.
/// @param path The path of the QBModel.
/// @param model The model to fill.
static bool loadQB(const fs::path& path, std::vector<QBMatrix>& model)
{
    auto* file = fopen(path.string().c_str(), "rb");
    if (file == nullptr)
    {
        return false;
    }

    auto stream = memory::StandardStream(file, true);
    return parseQB(model, stream);
}

/// Saves the given palette to the given path.
/// @param path The path of the palette.
/// @param palette The palette to save.
static bool savePalette(const fs::path& path, const VoxelPalette& palette)
{
    auto* file = fopen(path.string().c_str(), "wb");
    if (file == nullptr)
    {
        return false;
    }

    auto stream = memory::StandardStream(file, true);
    if (!palette.writeTo(stream))
    {
        std::cerr << "Failed to serialize palette." << std::endl;
        return false;
    }

    return true;
}

/// Saves the given grid to the given path.
/// @param path The path of the grid.
/// @param grid The grid to export.
static bool saveGrid(const fs::path& path, const VoxelGrid& grid)
{
    auto* file = fopen(path.string().c_str(), "wb");
    if (file == nullptr)
    {
        return false;
    }

    auto stream = memory::StandardStream(file, true);
    if (!grid.writeTo(stream))
    {
        std::cerr << "Failed to serialize grid." << std::endl;
        return false;
    }

    return true;
}

/// Runs the converter from the command line options.
/// @param options The command line options.
/// @return True if the conversion was successful, false otherwise.
static bool convert(const ConvertOptions& options)
{
    // First, load the palette.
    VoxelPalette palette;
    if (!options.palette.empty())
    {
        if (!loadPalette(options.palette, palette))
        {
            if (options.write && options.verbose)
            {
                std::cerr << "Failed to load palette: write enabled, creating new palette." << std::endl;
            }
            else if (!options.write)
            {
                std::cerr << "Failed to load palette: write disabled & file " << options.palette << " not found."
                          << std::endl;
                return false;
            }
        }
    }

    // Then, parse the Qubicle file.
    if (options.input.extension() != ".qb")
    {
        std::cerr << "Failed to load model: unsupported format " << options.input.extension() << "." << std::endl;
    }

    std::vector<QBMatrix> model;
    if (!loadQB(options.input, model))
    {
        std::cerr << "Failed to load model: " << options.input << " not found." << std::endl;
        return false;
    }

    if (options.verbose)
    {
        std::cerr << "Found " << model.size() << " QB matrices." << std::endl;
        for (std::size_t i = 0; i < model.size(); i++)
        {
            std::cerr << "Matrix " << i << ":" << std::endl;
            std::cerr << "- Position: " << model[i].position.x << " " << model[i].position.y << " "
                      << model[i].position.z << std::endl;
            std::cerr << "- Grid size: " << model[i].grid.size().x << "x" << model[i].grid.size().y << "x"
                      << model[i].grid.size().z << std::endl;
            std::cerr << "- Palette size: " << model[i].palette.size() << std::endl;
        }

        if (options.palette.empty())
        {
            return true;
        }
    }

    if (options.grids.empty())
    {
        std::cerr << "No output grids specified, not doing anything." << std::endl;
        return true;
    }

    // Iterate over the grids which will be exported.
    for (std::size_t i = 0; i < model.size(); ++i)
    {
        if (options.grids.contains(i))
        {
            auto path = options.grids.at(i);
            if (std::filesystem::exists(path) && !options.force)
            {
                std::cerr << "Output file " << path << " already exists. Continue anyway (y/n)?" << std::endl;
                std::string ans;
                std::cin >> ans;
                if (ans != "y" && ans != "Y")
                {
                    return false;
                }
            }

            if (options.verbose)
            {
                std::cout << "Writing grid " << i << " to " << path << std::endl;
            }

            // If write is enabled, merge this palette with the main one.
            if (options.write)
            {
                palette.merge(model[i].palette, options.similarity);
            }

            // Convert the grid to the main palette.
            if (!model[i].grid.convert(model[i].palette, palette, options.similarity))
            {
                std::cerr << "Failed to convert grid " << i << " from its palette to the palette chosen." << std::endl;

                if (options.write)
                {
                    std::cerr << "The palette is already full, so extra materials will not be added." << std::endl;
                    assert(palette.size() == UINT16_MAX);
                }
                else
                {
                    std::cerr << "Try enabling write (-w) so that the palette can be updated with extra materials."
                              << std::endl;
                }

                return false;
            }

            // Save the grid to the given path.
            if (!saveGrid(path, model[i].grid))
            {
                std::cerr << "Failed to save grid " << i << " to " << path << "." << std::endl;
                return false;
            }
        }
    }

    // Save the palette if write is enabled.
    if (options.write)
    {
        if (options.verbose)
        {
            std::cout << "Writing palette to " << options.palette << std::endl;
        }

        if (!savePalette(options.palette, palette))
        {
            std::cerr << "Failed to save palette to " << options.palette << "." << std::endl;
            return false;
        }
    }

    return true;
}

int runConvert(int argc, char** argv)
{
    // Parse command line arguments.
    ConvertOptions options = {};
    if (!parseArguments(argc, argv, options))
    {
        printHelp();
        return 1;
    }
    if (options.help)
    {
        printHelp();
        return 0;
    }

    // Convert the input file.
    if (!convert(options))
    {
        std::cerr << "Failed to convert file." << std::endl;
        return 1;
    }

    return 0;
}
