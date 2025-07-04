#define TEMPLATES_DIR "templates/projects"
#define QUADRADOS_DEFAULT_TEMPLATE "simple"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "tools.hpp"

namespace fs = std::filesystem;

struct Options
{
    fs::path projectPath = ".";
    std::string templateName = QUADRADOS_DEFAULT_TEMPLATE;
};

static void printHelp()
{
    std::cerr << "Usage: quadrados [GLOBAL OPTIONS] init <PROJECT NAME> [OPTIONS]" << std::endl;
    std::cerr << "Options:" << std::endl;
    std::cerr << "  -p <PATH>          Specifies the path of project directory." << std::endl;
    std::cerr << "  -t <TEMPLATE_NAME> Set the template to use." << std::endl;
    std::cerr << "  -l                 Lists avaliable templates." << std::endl;
    std::cerr << "Global Options:" << std::endl;
    std::cerr << "  -D <DIR>           Specifies the base directory for templates/assets." << std::endl;
}

std::vector<std::string> getTemplateOptions(const GlobalArgs& ga)
{
    std::vector<std::string> templates;
    fs::path templatesPath = fs::path(ga.installDir) / "templates";
    for (const auto& entry : fs::directory_iterator(templatesPath))
    {
        if (entry.is_directory())
        {
            templates.push_back(entry.path().filename().string());
        }
    }
    return templates;
}

bool templateExists(const std::string& temp, const GlobalArgs& ga)
{
    std::vector<std::string> templateList = getTemplateOptions(ga);
    return std::find(templateList.begin(), templateList.end(), temp) != templateList.end();
}

int parseArguments(int argc, char** argv, Options& options, GlobalArgs& ga)
{
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "-p" && i + 1 < argc)
        {
            options.projectPath = argv[++i];
        }
        else if (arg == "-t" && i + 1 < argc)
        {
            std::string temp = argv[++i];
            options.templateName = temp;
            std::cerr << "Using template: " << temp << std::endl;
            if (!templateExists(temp, ga))
            {
                std::cerr << "Error: " << temp << " is not a valid template" << std::endl;
                return -1;
            }
        }
        else if (arg == "-p" || arg == "-t")
        {
            std::cerr << "Error: no argument provided for " << arg << std::endl;
            return -1;
        }
    }

    return 0;
}

int copyTemplateFiles(const fs::path& source, const fs::path& destination)
{
    try
    {
        fs::copy(source, destination, fs::copy_options::recursive);
    }
    catch (const fs::filesystem_error&)
    {
        std::cerr << "Error: unexpected filesystem error" << std::endl;
        return -1;
    }
    return 0;
}

int createProjectDirectory(const fs::path& dirPath)
{
    try
    {
        if (!fs::exists(dirPath))
        {
            fs::create_directory(dirPath);
        }
        else
        {
            std::cerr << "Error: directory already exists" << std::endl;
            return -1;
        }
    }
    catch (const fs::filesystem_error&)
    {
        std::cerr << "Error: unexpected filesystem error" << std::endl;
        return -1;
    }

    return 0;
}

int runInit(int argc, char** argv, GlobalArgs& ga)
{
    Options options;
    if (argc == 0)
    {
        printHelp();
        return -1;
    }

    // parse the list command
    if (std::string(argv[0]) == "-l")
    {
        std::cerr << "Avaliable templates:" << std::endl;
        for (const std::string& name : getTemplateOptions(ga))
        {
            std::cerr << name << std::endl;
        }
        return 0;
    }

    fs::path projectDirectory = argv[0];
    if (parseArguments(argc, argv, options, ga) == -1)
    {
        printHelp();
        return -1;
    }

    if (!fs::exists(options.projectPath) || !fs::is_directory(options.projectPath))
    {
        std::cerr << "Error: specified path does not exist or is not a directory" << std::endl;
        return -1;
    }

    fs::path templatePath = fs::path(ga.installDir) / TEMPLATES_DIR / options.templateName;
    fs::path destinationPath = options.projectPath / projectDirectory;

    int createRes = createProjectDirectory(destinationPath);
    if (createRes < 0)
    {
        return -1;
    }

    int copyRes = copyTemplateFiles(templatePath, destinationPath);
    if (copyRes == -1)
    {
        return -1;
    }

    std::cerr << projectDirectory << " successfully created" << std::endl;
    return 0;
}
