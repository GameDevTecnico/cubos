#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "tools.hpp"

namespace fs = std::filesystem;

/// The input options of the program.
struct EmbedOptions
{
    std::string name;       ///< The name of the output data.
    fs::path input = "";    ///< The input file path.
    bool recursive = false; ///< Whether to recursively embed all files.
    bool verbose = false;   ///< Enables verbose mode.
    bool help = false;      ///< Prints the help message.
};

/// Prints the help message of the program.
static void printHelp()
{
    std::cerr << "Usage: quadrados [GLOBAL OPTIONS] embed [OPTIONS] <INPUT>" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -n <name>    Sets the name of the output data." << std::endl;
    std::cerr << "  -r           Recursively embed all files in the input directory." << std::endl;
    std::cerr << "  -v           Enables verbose mode." << std::endl;
    std::cerr << "  -h           Prints this help message." << std::endl;
    std::cerr << "Global Options:" << std::endl;
    std::cerr << "  -D <DIR>     Specifies the base directory for templates/assets." << std::endl;
}

/// Parses the command line arguments.
/// @param argc The number of arguments.
/// @param argv The arguments.
/// @param options The options to fill.
/// @return True if the arguments were parsed successfully, false otherwise.
static bool parseArguments(int argc, char** argv, EmbedOptions& options)
{
    bool foundInput = false;

    // Iterate over the arguments.
    for (int i = 0; i < argc; ++i)
    {
        if (std::string(argv[i]) == "-n")
        {
            if (i + 1 < argc)
            {
                options.name = argv[i + 1];
                i++;
            }
            else
            {
                std::cerr << "Missing argument for -n." << std::endl;
                return false;
            }
        }
        else if (std::string(argv[i]) == "-r")
        {
            options.recursive = true;
        }
        else if (std::string(argv[i]) == "-v")
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
    return true;
}

/// Escapes a string into valid C++ string content.
/// @param str The string to convert.
/// @return The escaped string.
static std::string escapeString(const std::string& str)
{
    // Escape all characters that need to be escaped.
    std::string escaped;
    for (char i : str)
    {
        if (i == '\\' || i == '"')
        {
            escaped += '\\';
        }
        escaped += i;
    }

    return escaped;
}

/// Stores info obtained from scanning the input file/directory.
struct ScanEntry
{
    std::string name;    ///< The name of the file.
    fs::path path;       ///< The path of the file.
    bool directory;      ///< Whether the file is a directory.
    std::size_t parent;  ///< The ID of the parent directory.
    std::size_t child;   ///< The ID of the child directory.
    std::size_t sibling; ///< The ID of the next sibling directory.
};

/// State required for the embedding process.
struct State
{
    const EmbedOptions& options; ///< The options of the program.
    std::ostream& out;           ///< The output stream.

    std::string name;               ///< The name of the output data.
    std::vector<ScanEntry> entries; ///< The scanned entries.
};

/// Scans the input file/directory and stores the result in the state.
/// @param state The state to fill.
/// @param id The ID of the file to scan.
/// @return True if the scan was successful, false otherwise.
static bool scanEntries(State& state, std::size_t id)
{
    // Check if this is the root entry.
    if (id == 0)
    {
        bool dir = fs::is_directory(state.options.input);
        state.entries.push_back({"", state.options.input, dir, 0, 0, 0});
        return scanEntries(state, 1);
    }

    // Check if the entry is a directory.
    if (state.entries[id - 1].directory)
    {
        if (!state.options.recursive)
        {
            if (state.options.verbose)
            {
                std::cout << "Ignoring directory contents of '" << state.entries[id - 1].path.string()
                          << "' since the recursive option was not set" << std::endl;
            }
            return true;
        }

        if (state.options.verbose)
        {
            std::cout << "Scanning directory '" << state.entries[id - 1].path.string() << "'..." << std::endl;
        }

        // Scan the directory entries.
        std::size_t lastChildId = 0;
        for (fs::directory_iterator it(state.entries[id - 1].path), end; it != end; ++it)
        {
            if (!fs::is_directory(it->path()) && !fs::is_regular_file(it->path()))
            {
                if (state.options.verbose)
                {
                    std::cout << "Ignoring '" << it->path().string() << "' since it is neither a directory nor a file"
                              << std::endl;
                }
                continue;
            }

            // Check if the entry is a directory.
            bool dir = fs::is_directory(it->path());
            state.entries.push_back({it->path().filename().string(), it->path(), dir, id, 0, 0});
            if (state.entries[id - 1].child == 0)
            {
                state.entries[id - 1].child = state.entries.size();
            }
            if (lastChildId != 0)
            {
                state.entries[lastChildId - 1].sibling = state.entries.size();
            }
            lastChildId = state.entries.size();

            // Scan the file recursively.
            if (!scanEntries(state, state.entries.size()))
            {
                return false;
            }
        }
    }
    else if (state.options.verbose)
    {
        std::cout << "Scanned file '" << state.entries[id - 1].path.string() << "'..." << std::endl;
    }

    return true;
}

/// Embeds the binary data of a file into the output stream.
/// @param state The state of the embedding process.
/// @param id The id of the file.
/// @return True if the file was embedded successfully, false otherwise.
static bool embedFileData(State& state, std::size_t id)
{
    if (state.entries[id - 1].directory)
    {
        // Embed the child files data.
        std::size_t child = state.entries[id - 1].child;
        while (child != 0)
        {
            if (!embedFileData(state, child))
            {
                return false;
            }
            child = state.entries[child - 1].sibling;
        }
    }
    else
    {
        if (state.options.verbose)
        {
            std::cerr << "Embedding file data of '" << state.entries[id - 1].path.string() << "'" << std::endl;
        }

        // Open the file.
        std::ifstream file(state.entries[id - 1].path, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file '" << state.entries[id - 1].path.string() << "'." << std::endl;
            return false;
        }

        state.out << "static const uint8_t fileData" << id << "[] = { ";

        // Write the file data.
        while (file.good())
        {
            char buffer[1024];
            file.read(buffer, sizeof(buffer));
            for (std::size_t i = 0; i < static_cast<std::size_t>(file.gcount()); ++i)
            {
                state.out << "0x" << std::hex << static_cast<uint32_t>(buffer[i]) << ", ";
            }
        }

        state.out << "};" << std::endl;

        file.close();
    }

    return true;
}

/// Embeds a file entry into the output stream.
/// @param state The state of the embedding process.
/// @param id The id of the file entry.
static bool embedFileEntry(State& state, std::size_t id)
{
    if (state.options.verbose)
    {
        std::cerr << "Embedding entry of '" << state.entries[id - 1].path.string() << "'." << std::endl;
    }

    // Embed the file entry.
    state.out << "    { \"" << escapeString(state.entries[id - 1].name) << "\", ";
    state.out << (state.entries[id - 1].directory ? "true, " : "false, ");
    state.out << state.entries[id - 1].parent << ", ";
    state.out << state.entries[id - 1].sibling << ", ";
    state.out << state.entries[id - 1].child << ", ";
    if (state.entries[id - 1].directory)
    {
        state.out << "nullptr, 0 }," << std::endl;
    }
    else
    {
        state.out << "fileData" << id << ", ";
        state.out << "sizeof(fileData" << id << ") }," << std::endl;
    }

    if (state.entries[id - 1].directory)
    {
        // Embed the child entries.
        std::size_t child = state.entries[id - 1].child;
        while (child != 0)
        {
            if (!embedFileEntry(state, child))
            {
                return false;
            }
            child = state.entries[child - 1].sibling;
        }
    }

    return true;
}

/// Runs the generator from the command line options.
/// @param options The command line options.
/// @return True if the embedding was successful, false otherwise.
static bool generate(const EmbedOptions& options)
{
    // Initialize the state.
    State state = {options, std::cout, options.name, {}};
    if (state.name.empty())
    {
        state.name = fs::path(options.input).filename().string();
        if (options.verbose)
        {
            std::cerr << "No name specified, using '" << state.name << "'." << std::endl;
        }
    }

    // Scan the input file/directory.
    if (!scanEntries(state, 0))
    {
        std::cerr << "Failed to scan the input file/directory." << std::endl;
        return false;
    }

    // Write the file header.
    state.out << "// This file was generated by quadrados embed." << std::endl;
    state.out << "// Do not edit this file." << std::endl;
    state.out << "//" << std::endl;
    state.out << "// To use this file in your project, you just need to link" << std::endl;
    state.out << "// this source file by adding it to your source files. To" << std::endl;
    state.out << "// use it in an EmbeddedArchive, you just need to construct" << std::endl;
    state.out << "// an EmbeddedArchive with the data name '" << state.name << "'" << std::endl;
    state.out << std::endl;
    state.out << "#include <cubos/core/data/fs/embedded_archive.hpp>" << std::endl;
    state.out << std::endl;
    state.out << "using cubos::core::data::EmbeddedArchive;" << std::endl;
    state.out << std::endl;

    // Write the data of the files.
    if (!embedFileData(state, 1))
    {
        std::cerr << "Failed to embed file datas of '" << options.input << "'." << std::endl;
        return false;
    }
    state.out << std::endl;

    // Write the file entries.
    state.out << "static const EmbeddedArchive::Data::Entry entries[] = {" << std::endl;
    if (!embedFileEntry(state, 1))
    {
        std::cerr << "Failed to embed file entries of '" << options.input << "'." << std::endl;
        return false;
    }
    state.out << "};" << std::endl;
    state.out << std::endl;

    // Write the data definition struct.

    state.out << "static const EmbeddedArchive::Data embeddedArchiveData = {" << std::endl;
    state.out << "    entries," << std::endl;
    state.out << "    sizeof(entries) / sizeof(entries[0])," << std::endl;
    state.out << "};" << std::endl;
    state.out << std::endl;

    // Write the data register.

    state.out << "class DataRegister" << std::endl;
    state.out << "{" << std::endl;
    state.out << "public:" << std::endl;
    state.out << "    DataRegister()" << std::endl;
    state.out << "    {" << std::endl;
    state.out << "        EmbeddedArchive::registerData(\"" << escapeString(state.name) << "\", embeddedArchiveData);"
              << std::endl;
    state.out << "    }" << std::endl;
    state.out << "};" << std::endl;
    state.out << std::endl;
    state.out << "static DataRegister dataRegister;" << std::endl;

    return true;
}

int runEmbed(int argc, char** argv, GlobalArgs& /*ga*/)
{
    // Parse command line arguments.
    EmbedOptions options = {};
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

    // Generate the file.
    if (!generate(options))
    {
        std::cerr << "Failed to embed file." << std::endl;
        return 1;
    }

    return 0;
}
