#include "tools.hpp"

#include <iostream>
#include <string>

/// Prints the help message of the program.
int runHelp(int, char**)
{
    std::cerr << "Usage: cubinhos <TOOL>" << std::endl;
    std::cerr << "Tools:" << std::endl;
    for (size_t i = 0; i < sizeof(tools) / sizeof(tools[0]); ++i)
    {
        std::cerr << "  " << tools[i].name << std::endl;
    }
    return 0;
}

int main(int argc, char** argv)
{
    // Parse command line arguments.
    if (argc >= 2)
    {
        // Find the desired tool.
        for (size_t i = 0; i < sizeof(tools) / sizeof(tools[0]); ++i)
        {
            if (tools[i].name == argv[1])
            {
                return tools[i].run(argc - 2, argv + 2);
            }
        }
    }

    runHelp(argc, argv);
    return 1;
}
