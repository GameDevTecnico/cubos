/// @file
/// @brief Class @ref cubos::core::geom::Box.
/// @ingroup core-geom

#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cubos/core/reflection/reflect.hpp>

namespace cubos::core::geom
{
    /// @brief Represents a box shape.
    /// @ingroup core-geom
    struct CUBOS_CORE_API Box
    {
        CUBOS_REFLECT;

        glm::vec3 halfSize{0.5F}; ///< Half size of the box.

        /// @brief Computes two opposite corners of the box on the major diagonal.
        /// @param corners Array to store the two corners in.
        void diag(glm::vec3 corners[2]) const
        {
            corners[0] = {-halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {halfSize.x, halfSize.y, halfSize.z};
        }

        /// @brief Computes four corners of the box, one for each diagonal.
        /// @param corners Array to store the three corners in.
        void corners4(glm::vec3 corners[4]) const
        {
            corners[0] = {halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {-halfSize.x, halfSize.y, -halfSize.z};
            corners[2] = {-halfSize.x, -halfSize.y, halfSize.z};
            corners[3] = {halfSize.x, halfSize.y, halfSize.z};
        }

        /// @brief Computes the eight corners of the box, opposite corners are adjacent in the
        /// array.
        /// @param corners Array to store the eight corners in.
        void corners(glm::vec3 corners[8]) const
        {
            corners[0] = {-halfSize.x, -halfSize.y, -halfSize.z};
            corners[1] = {halfSize.x, halfSize.y, halfSize.z};
            corners[2] = {-halfSize.x, -halfSize.y, halfSize.z};
            corners[3] = {halfSize.x, halfSize.y, -halfSize.z};
            corners[4] = {-halfSize.x, halfSize.y, -halfSize.z};
            corners[5] = {halfSize.x, -halfSize.y, halfSize.z};
            corners[6] = {-halfSize.x, halfSize.y, halfSize.z};
            corners[7] = {halfSize.x, -halfSize.y, -halfSize.z};
        }

        /// @brief Computes the enclosing faces of the corner.
        /// @param vertexIndex Index of the corner, according to the returned by corners.
        /// @param faces Array to store the three face indexes in, according to the returned by faces.
        static void enclosingFaces(int vertexIndex, int faces[3])
        {
            if (vertexIndex == 0)
            {
                faces[0] = 0;
                faces[1] = 3;
                faces[2] = 5;
            }
            else if (vertexIndex == 1)
            {
                faces[0] = 1;
                faces[1] = 2;
                faces[2] = 4;
            }
            else if (vertexIndex == 2)
            {
                faces[0] = 1;
                faces[1] = 3;
                faces[2] = 5;
            }
            else if (vertexIndex == 3)
            {
                faces[0] = 0;
                faces[1] = 2;
                faces[2] = 4;
            }
            else if (vertexIndex == 4)
            {
                faces[0] = 0;
                faces[1] = 2;
                faces[2] = 5;
            }
            else if (vertexIndex == 5)
            {
                faces[0] = 1;
                faces[1] = 3;
                faces[2] = 4;
            }
            else if (vertexIndex == 6)
            {
                faces[0] = 1;
                faces[1] = 2;
                faces[2] = 5;
            }
            else
            {
                faces[0] = 0;
                faces[1] = 3;
                faces[2] = 4;
            }
        }

        /// @brief Computes the edges of a box. An edge is a pair of the corner indexes that compose it.
        /// @param edges Array to store the twelve edges in.
        static void edges(glm::ivec2 edges[12])
        {
            edges[0] = {0, 4};
            edges[1] = {4, 3};
            edges[2] = {3, 7};
            edges[3] = {5, 1};
            edges[4] = {1, 6};
            edges[5] = {6, 2};
            edges[6] = {1, 3};
            edges[7] = {0, 7};
            edges[8] = {7, 5};
            edges[9] = {2, 0};
            edges[10] = {5, 2};
            edges[11] = {6, 4};
        }

        /// @brief Computes the faces adjacent to an edge.
        /// @param edgeIndex Index of the edge.
        /// @param faces Array to store the two faces in.
        static void edgeAdjacentFaces(int edgeIndex, int faces[2])
        {
            if (edgeIndex == 0)
            {
                faces[0] = 0;
                faces[1] = 5;
            }
            else if (edgeIndex == 1)
            {
                faces[0] = 0;
                faces[1] = 2;
            }
            else if (edgeIndex == 2)
            {
                faces[0] = 0;
                faces[1] = 4;
            }
            else if (edgeIndex == 3)
            {
                faces[0] = 1;
                faces[1] = 4;
            }
            else if (edgeIndex == 4)
            {
                faces[0] = 1;
                faces[1] = 2;
            }
            else if (edgeIndex == 5)
            {
                faces[0] = 1;
                faces[1] = 5;
            }
            else if (edgeIndex == 6)
            {
                faces[0] = 2;
                faces[1] = 4;
            }
            else if (edgeIndex == 7)
            {
                faces[0] = 0;
                faces[1] = 3;
            }
            else if (edgeIndex == 8)
            {
                faces[0] = 3;
                faces[1] = 4;
            }
            else if (edgeIndex == 9)
            {
                faces[0] = 3;
                faces[1] = 5;
            }
            else if (edgeIndex == 10)
            {
                faces[0] = 1;
                faces[1] = 3;
            }
            else
            {
                faces[0] = 2;
                faces[1] = 5;
            }
        }

        /// @brief Computes the edges of a face.
        /// @param faceIndex Index of the face.
        /// @param edges Array to store the four edges in.
        static void faceEdges(int faceIndex, int edges[4])
        {
            if (faceIndex == 0)
            {
                edges[0] = 0;
                edges[1] = 1;
                edges[2] = 2;
                edges[3] = 7;
            }
            else if (faceIndex == 1)
            {
                edges[0] = 3;
                edges[1] = 4;
                edges[2] = 5;
                edges[3] = 10;
            }
            else if (faceIndex == 2)
            {
                edges[0] = 4;
                edges[1] = 6;
                edges[2] = 1;
                edges[3] = 11;
            }
            else if (faceIndex == 3)
            {
                edges[0] = 7;
                edges[1] = 8;
                edges[2] = 10;
                edges[3] = 9;
            }
            else if (faceIndex == 4)
            {
                edges[0] = 3;
                edges[1] = 8;
                edges[2] = 2;
                edges[3] = 6;
            }
            else
            {
                edges[0] = 5;
                edges[1] = 11;
                edges[2] = 0;
                edges[3] = 9;
            }
        }

        /// @brief Computes the faces with regards to the corner indexes that compose them. In conter-clock wise winding
        /// order.
        /// @param faces Array to store the six faces in.
        static void faces(glm::ivec4 faces[6])
        {
            // in ccw winding order
            faces[0] = {0, 4, 3, 7};
            faces[1] = {5, 1, 6, 2};
            faces[2] = {6, 1, 3, 4};
            faces[3] = {0, 7, 5, 2};
            faces[4] = {1, 5, 7, 3};
            faces[5] = {2, 6, 4, 0};
        }

        /// @brief Computes the normal of each face. Same order as faces.
        /// @param normals Array to store the normals in.
        static void faceNormals(glm::vec3 normals[6])
        {
            // in ccw winding order
            normals[0] = {0.0F, 0.0F, -1.0F};
            normals[1] = {0.0F, 0.0F, 1.0F};
            normals[2] = {0.0F, 1.0F, 0.0F};
            normals[3] = {0.0F, -1.0F, 0.0F};
            normals[4] = {1.0F, 0.0F, 0.0F};
            normals[5] = {-1.0F, 0.0F, 0.0F};
        }

        static void normals(glm::vec3 normals[3])
        {
            normals[0] = {0.0F, 1.0F, 0.0F};
            normals[1] = {0.0F, 0.0F, 1.0F};
            normals[2] = {1.0F, 0.0F, 0.0F};
        }
    };
} // namespace cubos::core::geom
