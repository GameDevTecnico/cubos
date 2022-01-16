#ifndef CUBOS_DATA_QB_PARSER_HPP
#define CUBOS_DATA_QB_PARSER_HPP

#include <cubos/memory/stream.hpp>
#include <cubos/gl/grid.hpp>
#include <cubos/gl/palette.hpp>

#include <glm/glm.hpp>

#include <vector>

namespace cubos::data
{
    /// Represents the data read from a matrix in a QB file.
    struct QBMatrix
    {
        gl::Grid grid;       ///< The grid of the matrix.
        gl::Palette palette; ///< The palette of the matrix.
        glm::ivec3 position; ///< The position of the matrix.
    };

    /// Parses a Qubicle file (.qb), pushing every matrix found in the file to the passed vector.
    /// @param matrices The matrices to output.
    /// @param stream The stream to read from.
    /// @return True if the file was parsed successfully, otherwise false.
    bool parseQB(std::vector<QBMatrix>& matrices, memory::Stream& stream);
} // namespace cubos::data

#endif // CUBOS_DATA_QB_PARSER_HPP
