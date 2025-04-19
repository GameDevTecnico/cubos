#pragma once

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
int runInit(int argc, char** argv);

static const Tool Tools[] = {
    {"help", runHelp},
    {"embed", runEmbed},
    {"convert", runConvert},
    {"init", runInit},
};
