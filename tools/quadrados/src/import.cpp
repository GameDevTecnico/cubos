#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>

#include "tools.hpp"

namespace fs = std::filesystem;

/// Generates a UUID-like random string (with hyphens),
/// consisting of digits [0-9] and lowercase letters [a-z].
/// @returns A 36-character string in the format xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
static std::string generateRandomStringUUID()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    const char charset[] = "0123456789"
                           "abcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);

    std::string uuid(36, ' ');
    for (int i = 0; i < 36; i++)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            uuid[i] = '-';
        }
        else
        {
            uuid[i] = charset[dis(gen)];
        }
    }
    return uuid;
}

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

    std::string uuid = generateRandomStringUUID();

    std::ofstream metaFile(metaPath);
    if (!metaFile)
    {
        std::cerr << "Error: could not create .meta file at " << metaPath << "\n";
        return -1;
    }

    metaFile << "{\n";
    metaFile << "    \"id\": \"" << uuid << "\"\n";
    metaFile << "}\n";
    metaFile.close();

    std::cerr << "Created " << metaPath << " with UUID: " << uuid << "\n";
    return 0;
}
