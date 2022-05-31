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

static const Tool tools[] = {{"help", runHelp}, {"embed", runEmbed}, {"convert", runConvert}};

#endif // CUBINHOS_TOOLS_HPP
