#pragma once

#include <string>

struct GlobalArgs {
    std::string installDir = QUADRADOS_INSTALL_PATH;
};
/// Stores info about a tool.
struct Tool
{
    std::string name;
    int (*run)(int, char**, GlobalArgs&);
};


int runHelp(int argc, char** argv, GlobalArgs& ga);
int runEmbed(int argc, char** argv, GlobalArgs& ga);
int runConvert(int argc, char** argv, GlobalArgs& ga);
int runInit(int argc, char** argv, GlobalArgs& ga);

static const Tool Tools[] = {
    {"help", runHelp},
    {"embed", runEmbed},
    {"convert", runConvert},
    {"init", runInit},
};
