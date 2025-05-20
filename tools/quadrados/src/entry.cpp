#include <filesystem>
#include <iostream>
#include <string>

#include "tools.hpp"

namespace fs = std::filesystem;

/// Prints the help message of the program.
int runHelp(int /*argc*/, char** /*argv*/, GlobalArgs& /*ga*/)
{
    std::cerr << "Usage: quadrados [OPTIONS] <TOOL>" << std::endl;
    std::cerr << "Tools:" << std::endl;
    for (const auto& tool : Tools)
    {
        std::cerr << "  " << tool.name << std::endl;
    }
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -D <install_directory> path to install directory" << std::endl;

    return 0;
}

/*
/   Checks if a string is a tool
*/
bool isTool(char* param)
{
    for (const auto& tool : Tools)
    {
        if (tool.name == param)
        {

            return true;
        }
    }

    return false;
}

/*
/   Returns the number of parsed args
/   and updates the GlobalArgs struct if needed
*/
int parseGlobalArgs(int argc, char** argv, GlobalArgs& ga)
{
    int nParsed = 0;
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-D" && i + 1 < argc)
        {
            ga.installDir = argv[++i];
            nParsed += 2;
        }
        else if (arg == "-D")
        {
            std::cerr << "Missing argument for option -D" << std::endl;
            return -1;
        }
        else if (isTool(argv[i]))
        {
            return nParsed;
        }
        else
        {
            std::cerr << "Unknown option " << argv[i] << std::endl;
            return -1;
        }
    }
    std::cerr << "Missing tool name" << std::endl;
    return -1;
}

int main(int argc, char** argv)
{
    // Parse global args
    GlobalArgs globalArgs;
    int parse = parseGlobalArgs(argc, argv, globalArgs);

    if (parse == -1)
    {
        runHelp(argc, argv, globalArgs);
        return -1;
    }

    if (argc - parse >= 2)
    {
        // Find the desired tool.
        for (const auto& tool : Tools)
        {
            if (tool.name == argv[parse + 1])
            {
                return tool.run(argc - parse - 2, argv + parse + 2, globalArgs);
            }
        }
    }

    runHelp(argc, argv, globalArgs);
    return 1;
}
