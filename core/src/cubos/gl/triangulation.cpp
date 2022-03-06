
#include <vector>
#include <tuple>
#include <cubos/gl/vertex.hpp>
#include <cubos/gl/grid.hpp>
#include <cubos/gl/triangulation.hpp>

using namespace cubos::gl;
using namespace glm;
using std::vector;

void add_triangle_from_quad(vector<Triangle>& triangles, glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft,
                            glm::vec3 topRight, uint16_t material_id)
{
    Triangle triangle_bl, triangle_tr;

    triangle_bl.v0.material = triangle_bl.v1.material = triangle_bl.v2.material = triangle_tr.v0.material =
        triangle_tr.v1.material = triangle_tr.v2.material = material_id;

    auto normal = glm::cross(topLeft - bottomLeft, bottomRight - topLeft);

    triangle_bl.v0.normal = triangle_bl.v1.normal = triangle_bl.v2.normal = triangle_tr.v0.normal =
        triangle_tr.v1.normal = triangle_tr.v2.normal = normal;

    triangle_bl.v0.position = topLeft;
    triangle_bl.v1.position = bottomLeft;
    triangle_bl.v2.position = bottomRight;

    triangle_tr.v0.position = topRight;
    triangle_tr.v1.position = topLeft;
    triangle_tr.v2.position = bottomRight;

    triangles.push_back(triangle_bl);
    triangles.push_back(triangle_tr);
}

void add_all_voxel_triangles(vector<Triangle>& triangles, glm::uvec3 position, uint16_t material_id)
{
    add_triangle_from_quad(triangles, position, position + glm::uvec3(1, 0, 0), position + glm::uvec3(1, 0, 0),
                           position + glm::uvec3(1, 1, 0), material_id);

    add_triangle_from_quad(triangles, position + glm::uvec3(1, 0, 1), position + glm::uvec3(0, 0, 1),
                           position + glm::uvec3(1, 1, 1), position + glm::uvec3(0, 1, 1), material_id);

    add_triangle_from_quad(triangles, position + glm::uvec3(1, 0, 0), position + glm::uvec3(1, 0, 1),
                           position + glm::uvec3(1, 1, 0), position + glm::uvec3(1, 1, 1), material_id);

    add_triangle_from_quad(triangles, position + glm::uvec3(0, 0, 1), position, position + glm::uvec3(0, 1, 1),
                           position + glm::uvec3(0, 1, 0), material_id);

    add_triangle_from_quad(triangles, position, position + glm::uvec3(0, 0, 1), position + glm::uvec3(1, 0, 0),
                           position + glm::uvec3(1, 0, 1), material_id);

    add_triangle_from_quad(triangles, position + glm::uvec3(1, 1, 0), position + glm::uvec3(1, 1, 1),
                           position + glm::uvec3(0, 1, 0), position + glm::uvec3(0, 1, 1), material_id);
}

vector<Triangle> Triangulation::Triangulate(Grid grid)
{
    glm::uvec3 grid_size = grid.getSize();
    vector<Triangle> triangles = vector<Triangle>();

    for (uint x = 0; x < grid_size.x; x++)
    {
        for (uint y = 0; y < grid_size.y; y++)
        {
            for (uint z = 0; z < grid_size.z; z++)
            {
                glm::uvec3 position = {x, y, z};
                uint16_t material_id = grid.get(position);

                if (material_id != 0)
                {
                    add_all_voxel_triangles(triangles, position, material_id);
                }
            }
        }
    }

    return triangles;
}