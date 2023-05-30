#pragma once

#include <cubos/core/memory/stream.hpp>

#include "../utils.hpp"

/// Dumps the contents of a stream into a string.
inline std::string dump(cubos::core::memory::Stream& stream)
{
    std::string contents;
    stream.readUntil(contents, nullptr);
    return contents;
}
