/// @file
/// @brief Contains the class for the Simplex shape.

#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace cubos::core::geom
{
    /// @brief A simplex shape - may either be empty, a point, a line, a triangle or a tetrahedron.
    struct Simplex
    {
        // TODO: We should implement something like boost::static_vector for this. See
        // https://github.com/GameDevTecnico/cubos/issues/419

        const std::vector<glm::vec3> points; ///< The points of the simplex.

        /// @brief Constructs an empty simplex.
        /// @returns The empty simplex.
        static Simplex empty()
        {
            return {{}};
        }

        /// @brief Constructs a point simplex.
        /// @param p The point.
        /// @returns The point simplex.
        static Simplex point(const glm::vec3& p)
        {
            return {{p}};
        }

        /// @brief Constructs a line simplex.
        /// @param p1 The first point.
        /// @param p2 The second point.
        /// @returns The line simplex.
        static Simplex line(const glm::vec3& p1, const glm::vec3& p2)
        {
            return {{p1, p2}};
        }

        /// @brief Constructs a triangle simplex.
        /// @param p1 The first point.
        /// @param p2 The second point.
        /// @param p3 The third point.
        /// @returns The triangle simplex.
        static Simplex triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
        {
            return {{p1, p2, p3}};
        }

        /// @brief Constructs a tetrahedron simplex.
        /// @param p1 The first point.
        /// @param p2 The second point.
        /// @param p3 The third point.
        /// @param p4 The fourth point.
        /// @returns The tetrahedron simplex.
        static Simplex tetrahedron(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4)
        {
            return {{p1, p2, p3, p4}};
        }
    };
} // namespace cubos::core::geom
