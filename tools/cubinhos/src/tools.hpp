#ifndef CUBINHOS_TOOLS_HPP
#define CUBINHOS_TOOLS_HPP

#include <string>

/// Stores info about a tool.
struct Tool
{
    std::string name;
    int (*run)(int, char**);
};

int runHelp(int argc, char** argv);
int runEmbed(int argc, char** argv);
int runConvert(int argc, char** argv);
int runGenerate(int argc, char** argv);

static const Tool Tools[] = {
    {"help", runHelp},
    {"embed", runEmbed},
    {"convert", runConvert},
    {"generate", runGenerate},
};

#endif // CUBINHOS_TOOLS_HPP
