#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace cubos::core::geom
{
    struct Simplex
    {
        // TODO: We should implement something like boost::static_vector for this. See
        // https://github.com/GameDevTecnico/cubos/issues/419

        const std::vector<glm::vec3> points; ///< The points of the simplex.

        static Simplex empty()
        {
            return {{}};
        }

        static Simplex point(const glm::vec3& p)
        {
            return {{p}};
        }

        static Simplex line(const glm::vec3& p1, const glm::vec3& p2)
        {
            return {{p1, p2}};
        }

        static Simplex triangle(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
        {
            return {{p1, p2, p3}};
        }

        static Simplex tetrahedron(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& p4)
        {
            return {{p1, p2, p3, p4}};
        }
    };
} // namespace cubos::core::geom
