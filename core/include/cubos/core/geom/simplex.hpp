/// @file
/// @brief Contains the class for the Simplex shape.

#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/data/deserializer.hpp>
#include <cubos/core/data/serializer.hpp>

namespace cubos::core::geom
{
    /// @brief A simplex shape - may either be empty, a point, a line, a triangle or a tetrahedron.
    struct Simplex
    {
        // TODO: We should implement something like boost::static_vector for this. See
        // https://github.com/GameDevTecnico/cubos/issues/419

        std::vector<glm::vec3> points{}; ///< The points of the simplex.

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

namespace cubos::core::data
{
    /// Serializes a simplex.
    /// @param ser The serializer to use.
    /// @param simplex The simplex to serialize.
    /// @param name The name of the simplex.
    void serialize(Serializer& ser, const geom::Simplex& simplex, const char* name)
    {
        ser.write(simplex.points, name);
    }

    /// Deserializes a simplex.
    /// @param des The deserializer to use.
    /// @param simplex The simplex to deserialize.
    void deserialize(Deserializer& des, geom::Simplex& simplex)
    {
        des.read(simplex.points);
    }
} // namespace cubos::core::data
