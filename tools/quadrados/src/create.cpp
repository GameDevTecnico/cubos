#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <uuid.h>

#include "tools.hpp"

namespace fs = std::filesystem;

/// The input options of the program.
struct CreateOptions
{
    std::vector<fs::path> assets; ///< The path of the assets to be created.
    bool verbose = false;         ///< Enables verbose mode.
    bool help = false;            ///< Prints the help message.
};

/// State required for the asset creating process.
struct State
{
    const CreateOptions& options;                           ///< The options of the program.
    std::mt19937& random;                                   ///< The random number generator used for generating uuids.
    std::unordered_map<std::string, std::string> templates; ///< Available templates.
};

/// Prints the help message of the program.
static void printHelp()
{
    std::cerr << "Usage: quadrados [GLOBAL OPTIONS] create [OPTIONS] <ASSETS>" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -v           Enables verbose mode." << std::endl;
    std::cerr << "  -h           Prints this help message." << std::endl;
    std::cerr << "Global Options:" << std::endl;
    std::cerr << "  -D <DIR>           Specifies the base directory for templates/assets." << std::endl;
}

/// Parses the command line arguments.
/// @param argc The number of arguments.
/// @param argv The arguments.
/// @param options The options to fill.
/// @return True if the arguments were parsed successfully, false otherwise.
static bool parseArguments(int argc, char** argv, CreateOptions& options)
{
    std::unordered_set<fs::path> seen; // To avoid duplicates

    for (int i = 0; i < argc; ++i)
    {
        if (std::string(argv[i]) == "-v")
        {
            options.verbose = true;
        }
        else if (std::string(argv[i]) == "-h")
        {
            options.help = true;
            return true;
        }
        else
        {
            // Avoid duplicates
            if (seen.emplace(argv[i]).second)
            {
                options.assets.emplace_back(argv[i]);
            }
        }
    }

    if (options.assets.empty())
    {
        std::cerr << "Missing asset file(s)." << std::endl;
        return false;
    }

    return true;
}

/// Gets all the available asset templates and stores them on state variable.
/// @param ga Global arguments.
/// @param state Current state of the program.
static void getTemplates(const GlobalArgs& ga, State& state)
{
    fs::path templatesPath = fs::path(ga.installDir) / "templates" / "assets";

    for (const auto& entry : fs::recursive_directory_iterator(templatesPath))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        const std::string& ext = entry.path().extension().string();
        state.templates[ext] = entry.path().string();
    }
}

/// Gets the template from a given file extension.
/// @param extension File extension.
/// @param state Current state of the program.
/// return Template file path.
static fs::path getTemplate(const std::string& extension, const State& state)
{
    if (state.templates.contains(extension))
    {
        return state.templates.at(extension);
    }

    return fs::path{};
}

/// Copies a template file to the desired location.
/// @param source The asset we want to copy.
/// @param destination The destination of the copied asset.
/// @return True if the file was copied successfully, false otherwise.
static bool copyTemplateFile(const fs::path& source, const fs::path& destination)
{
    try
    {
        fs::copy_file(source, destination);
    }
    catch (const fs::filesystem_error&)
    {
        return false;
    }

    return true;
}

/// Generates a new .meta file.
/// @param state The state of the creation process
/// @param asset The path of the asset
/// @return True if the .meta file was generated successfully, false otherwise.
static bool generateMeta(State& state, const fs::path& asset)
{
    fs::path metaFilePath = asset;
    metaFilePath += ".meta";
    std::ofstream metaFile(metaFilePath);
    if (!metaFile.is_open())
    {
        return false;
    }

    auto id = uuids::uuid_random_generator(state.random)();

    metaFile << "{" << std::endl;
    metaFile << R"(    "id": ")" << id << "\"" << std::endl;
    metaFile << "}" << std::endl;

    if (state.options.verbose)
    {
        std::cerr << "Created " << metaFilePath.string() << " with id: " << id << std::endl;
    }

    metaFile.close();
    return true;
}

/// Generates all asset files and their metas according to a template (if exists).
/// @param options The command line options.
/// @param ga Global arguments.
/// @return True if all the assets were created successfully, false otherwise.
static bool generate(const CreateOptions& options, const GlobalArgs& ga)
{
    // Initialize the UUID generator.
    std::random_device rd;
    auto seedData = std::array<int, std::mt19937::state_size>{};
    std::ranges::generate(seedData, std::ref(rd));
    std::seed_seq seq(seedData.begin(), seedData.end());
    auto random = std::mt19937(seq);

    State state = {.options = options, .random = random, .templates = {}};

    getTemplates(ga, state);

    for (const auto& asset : options.assets)
    {
        if (fs::exists(asset))
        {
            std::cerr << "Asset file " << asset.string() << " already exists. Continue anyway (y/n)?" << std::endl;
            std::string ans;
            std::cin >> ans;
            if (ans != "y" && ans != "Y")
            {
                std::cerr << "Ignoring file " << asset.string() << std::endl;
                continue;
            }
        }

        // Create directory if needed.
        fs::path parent = asset.parent_path();
        if (!parent.empty() && !fs::exists(parent))
        {
            fs::create_directories(parent);

            if (!fs::exists(parent))
            {
                std::cerr << "Failed to create directory: " << parent.string() << std::endl;
                continue;
            }

            if (options.verbose)
            {
                std::cerr << "Created directory: " << parent.string() << std::endl;
            }
        }

        auto templateFile = getTemplate(asset.extension().string(), state);
        if (!templateFile.empty())
        {
            if (!copyTemplateFile(templateFile, asset))
            {

                std::cerr << "Could not copy files from template directory " << templateFile.parent_path().string()
                          << std::endl;

                return false;
            }

            if (options.verbose)
            {
                std::cerr << "Created asset from template: " << asset.string() << std::endl;
            }
        }
        else
        {
            std::ofstream file(asset);
            if (!file.is_open())
            {
                std::cerr << "Failed to create asset: " << asset.string() << std::endl;
                continue;
            }

            if (options.verbose)
            {
                std::cerr << "Created asset: " << asset.string() << std::endl;
            }

            file.close();
        }

        if (!generateMeta(state, asset))
        {
            std::cerr << "Failed to create .meta file for: " << asset.string() << std::endl;
            continue;
        }
    }

    return true;
}

int runCreate(int argc, char** argv, GlobalArgs& ga)
{
    // Parse command line arguments.
    CreateOptions options = {};
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

    // Generate the new assets.
    if (!generate(options, ga))
    {
        return 1;
    }

    std::cerr << "Successfully created asset files." << std::endl;

    return 0;
}
