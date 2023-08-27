#include <iostream>
#include <string>

#include "tools.hpp"

/// Prints the help message of the program.
int runHelp(int /*argc*/, char** /*argv*/)
{
    std::cerr << "Usage: quadrados <TOOL>" << std::endl;
    std::cerr << "Tools:" << std::endl;
    for (const auto& tool : Tools)
    {
        std::cerr << "  " << tool.name << std::endl;
    }
    return 0;
}

int main(int argc, char** argv)
{
    // Parse command line arguments.
    if (argc >= 2)
    {
        // Find the desired tool.
        for (const auto& tool : Tools)
        {
            if (tool.name == argv[1])
            {
                return tool.run(argc - 2, argv + 2);
            }
        }
    }

    runHelp(argc, argv);
    return 1;
}
