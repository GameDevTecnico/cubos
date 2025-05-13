#include <filesystem>
#include <fstream>
#include <iostream>

#include "tools.hpp"

namespace fs = std::filesystem;

static void printHelp()
{
    std::cerr << "Usage: quadrados init <PROJECT NAME> [OPTIONS]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -p <PATH>   Specifies the path of project directory." << std::endl;
}

int parseArguments(int argc, char** argv, fs::path& projectPath)
{
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-p" && i + i < argc)
            projectPath = argv[++i];
        else if (arg == "-p")
            return -1;
    }

    return 0;
}

fs::path getTemplatePath()
{

    fs::path templatePath;

    std::cerr << QUADRADOS_DEV_TEMPLATE_DIR << std::endl;
    if (fs::exists(QUADRADOS_DEV_TEMPLATE_DIR))
    {
        templatePath = QUADRADOS_DEV_TEMPLATE_DIR;
    }
    else
    {
        templatePath = QUADRADOS_INSTALL_TEMPLATE_DIR;
    }

    return templatePath;
}

int copyTemplateFiles(fs::path source, fs::path destination)
{
    try
    {
        fs::copy(source, destination, fs::copy_options::recursive);
    }
    catch (const fs::filesystem_error& e)
    {
        return -1;
    }
    return 0;
}

int createProjectDirectory(fs::path dirPath)
{
    try
    {
        if (!fs::exists(dirPath))
            fs::create_directory(dirPath);
        else
            return -2;
    }
    catch (const fs::filesystem_error& e)
    {
        return -1;
    }

    return 0;
}

int runInit(int argc, char** argv)
{

    if (argc == 0)
        printHelp();
    fs::path projectDirectory = argv[0];
    fs::path projectPath = ".";

    if (parseArguments(argc, argv, projectPath) == -1)
    {
        printHelp();
        return -1;
    }

    if (!fs::exists(projectPath) || !fs::is_directory(projectPath))
    {
        std::cerr << "Error: specified path does not exist or is not a directory." << std::endl;
        return -1;
    }

    fs::path templatePath = getTemplatePath();
    fs::path destinationPath = projectPath / projectDirectory;

    int createRes = createProjectDirectory(destinationPath);
    if (createRes < 0)
    {
        if (createRes == -1)
            std::cerr << "Error: unexpected filesystem error" << std::endl;
        else if (createRes == -2)
            std::cerr << "Error: directory already exists" << std::endl;
        return -1;
    }

    int copyRes = copyTemplateFiles(templatePath, destinationPath);
    if (copyRes == -1)
    {
        std::cerr << "Error: unexpected filesystem error" << std::endl;
        return -1;
    }

    std::cerr << projectDirectory << " successfully created" << std::endl;
    return 0;
}
