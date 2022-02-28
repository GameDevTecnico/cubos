#ifndef CUBOS_GL_TRIANGULATION
#define CUBOS_GL_TRIANGULATION

#include <vector>
#include <cubos/gl/vertex.hpp>
#include <cubos/gl/grid.hpp>

namespace cubos::gl
{
    struct Triangle
    {
        cubos::gl::Vertex v0;
        cubos::gl::Vertex v1;
        cubos::gl::Vertex v2;
    };

    class Triangulation
    {

    public:
        static std::vector<Triangle> Triangulate(cubos::gl::Grid grid);
    };
} // namespace cubos::gl

#endif // CUBOS_GL_TRIANGULATION
