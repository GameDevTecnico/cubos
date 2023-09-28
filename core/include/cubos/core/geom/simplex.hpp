/// @file
/// @brief Class @ref cubos::core::geom::Simplex.
/// @ingroup core-geom

#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/data/old/deserializer.hpp>
#include <cubos/core/data/old/serializer.hpp>

namespace cubos::core::geom
{
    /// @brief Represents a simplex shape, which may either be empty, a point, a line, a triangle
    /// or a tetrahedron.
    /// @ingroup core-geom
    struct Simplex
    {
        /// @todo We should implement something like `boost::static_vector` for this. See #419.
        std::vector<glm::vec3> points{}; ///< Points of the simplex.

        /// @brief Constructs an empty simplex.
        /// @return Empty simplex.
        static Simplex empty()
        {
            return {{}};
        }

        /// @brief Constructs a point simplex.
        /// @param p Point.
        /// @return Point simplex.
        static Simplex point(const glm::vec3& p)
        {
            return {{p}};
        }

        /// @brief Constructs a line simplex.
        /// @param p1 First point.
        /// @param p2 Second point.
        /// @return Line simplex.
        static Simplex line(const glm::vec3& p1, const glm::vec3& p2)
        {
            return {{p1, p2}};
        }

        /// @brief Constructs a triangle simplex.
        /// @param p1 First point.
        /// @param p2 Second point.
        /// @param p3 Third point.
        /// @return Triangle simplex.
        static Simplex triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
        {
            return {{p1, p2, p3}};
        }

        /// @brief Constructs a tetrahedron simplex.
        /// @param p1 First point.
        /// @param p2 Second point.
        /// @param p3 Third point.
        /// @param p4 Fourth point.
        /// @return Tetrahedron simplex.
        static Simplex tetrahedron(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4)
        {
            return {{p1, p2, p3, p4}};
        }
    };
} // namespace cubos::core::geom

namespace cubos::core::data
{
    inline void serialize(Serializer& ser, const geom::Simplex& simplex, const char* name)
    {
        ser.write(simplex.points, name);
    }

    inline void deserialize(Deserializer& des, geom::Simplex& simplex)
    {
        des.read(simplex.points);
    }
} // namespace cubos::core::data
