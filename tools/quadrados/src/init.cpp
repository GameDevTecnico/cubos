#include "tools.hpp"

#include <filesystem>
#include <fstream>
#include <unistd.h>
#include <iostream>

#define MAX_PATH_SIZE 4096
#define TEMPLATE_PATH "tools"/"tesseratos"/"templates"/"simple"

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
        if (arg == "-p" && i + i < argc) projectPath = argv[++i];
        else if (arg == "-p") return -1;
    }

    return 0;
}

fs::path getExecutablePath()
{
    char buffer[MAX_PATH_SIZE];
    ssize_t count = readlink("/proc/self/exe", buffer, MAX_PATH_SIZE);
    if (count == -1) {
        throw std::runtime_error("Unable to get binary path");
    }
    fs::path executablePath = fs::path(std::string(buffer, count))
        .parent_path()
        .parent_path();

    return executablePath;
}

int copyTemplateFiles(fs::path source, fs::path destination) {
    std::cerr << source << std::endl;
    std::cerr << destination << std::endl;

    try {
        fs::copy(source, destination, fs::copy_options::recursive);

    } catch (const fs::filesystem_error& e) {
        return -1;
    }
    return 0;
}

int createProjectDirectory(fs::path dirPath)
{
    try {
        if (!fs::exists(dirPath)) fs::create_directory(dirPath);
        else return -2;

    } catch (const fs::filesystem_error& e) {
        return -1;
    }

    return 0;
}

int runInit(int argc, char** argv)
{

    if (argc == 0) printHelp();
    fs::path projectDirectory = argv[0];
    fs::path projectPath = ".";

    if (parseArguments(argc, argv, projectPath) == -1)
    {
        printHelp();
        return -1;
    }

    fs::path executablePath = getExecutablePath();
    fs::path templatePath = executablePath/TEMPLATE_PATH;
    fs::path destinationPath = projectPath/projectDirectory;

    int createRes = createProjectDirectory(destinationPath);
    if (createRes < 0)
    {
        if (createRes == -1) std::cerr << "Error: unexpected filesystem error" << std::endl;
        else if (createRes == -2) std::cerr << "Error: directory already exists" << std::endl;
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
