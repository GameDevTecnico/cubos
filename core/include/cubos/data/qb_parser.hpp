#ifndef CUBOS_DATA_QB_PARSER_HPP
#define CUBOS_DATA_QB_PARSER_HPP

#include <cubos/memory/stream.hpp>
#include <cubos/gl/grid.hpp>
#include <cubos/gl/palette.hpp>

namespace cubos::data
{
    /// Parses a Qubicle file (.qb), outputting a gl::Grid and a gl::Palette.
    /// @param grid The grid to output.
    /// @param palette The palette to output.
    /// @param stream The stream to read from.
    /// @return True if the file was parsed successfully, otherwise false.
    bool parseQB(gl::Grid& grid, gl::Palette& palette, memory::Stream& stream);
} // namespace cubos::data

#endif // CUBOS_DATA_QB_PARSER_HPP
