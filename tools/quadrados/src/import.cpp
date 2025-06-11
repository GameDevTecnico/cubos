#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

#include <uuid.h>

#include "tools.hpp"

namespace fs = std::filesystem;

/// `import` command for `quadrados`:
/// - Verifies the asset exists.
/// - If `<asset>.meta` already exists, just reports.
/// - Otherwise, generates a UUID, creates `<asset>.meta` with JSON:
///   {
///     "id": "<UUID>"
///   }
///
/// @param argc Number of arguments passed after `import`.
/// @param argv Array of argument strings.
/// @param ga   Global arguments (not used here).
/// @returns 0 on success, <0 on error.
int runImport(int argc, char** argv, GlobalArgs& /*ga*/)
{
    if (argc != 1)
    {
        std::cerr << "Usage: quadrados import <file>\n";
        return -1;
    }

    fs::path assetPath = argv[0];
    if (!fs::exists(assetPath))
    {
        std::cerr << "Error: file does not exist: " << assetPath << "\n";
        return -1;
    }

    fs::path metaPath = assetPath;
    metaPath += ".meta";

    if (fs::exists(metaPath))
    {
        std::cerr << "Meta file already exists: " << metaPath << "\n";
        return 0;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    auto uuid = uuids::uuid_random_generator(gen)();

    std::ofstream metaFile(metaPath);
    if (!metaFile)
    {
        std::cerr << "Error: could not create .meta file at " << metaPath << "\n";
        return -1;
    }

    metaFile << "{\n";
    metaFile << R"(    "id": ")" << uuid << "\"\n";
    metaFile << "}\n";
    metaFile.close();

    std::cerr << "Created " << metaPath << " with UUID: " << uuid << "\n";
    return 0;
}
