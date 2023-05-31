#pragma once

#include <filesystem>

#include <cubos/core/memory/stream.hpp>

#include "../utils.hpp"

/// Dumps the contents of a stream into a string.
inline std::string dump(cubos::core::memory::Stream& stream)
{
    std::string contents;
    stream.readUntil(contents, nullptr);
    return contents;
}

/// Returns a path to a temporary location to use for testing.
/// It is guaranteed that the path does not exist, but its parent directory does.
/// Always returns the same path for the same test, but erases the path before returning it.
inline std::filesystem::path genTempPath(std::string name = "")
{
    auto tmp = std::filesystem::temp_directory_path();
    auto path = tmp / ("cubos-core-tests" + (name.empty() ? "" : "-" + name));
    std::filesystem::remove_all(path);
    return path;
}
