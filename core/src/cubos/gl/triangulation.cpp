
#include <vector>
#include <tuple>
#include <cubos/log.hpp>
#include <cubos/gl/vertex.hpp>
#include <cubos/gl/grid.hpp>
#include <cubos/gl/triangulation.hpp>

using namespace cubos::gl;
using namespace glm;
using std::vector;

void add_triangle_from_quad(vector<Triangle>& triangles, glm::vec3 bottomLeft, glm::vec3 bottomRight, glm::vec3 topLeft,
                            glm::vec3 topRight, uint16_t material_id)
{
    cubos::logDebug("Add Quad");
    cubos::logDebug("{},{},{}", bottomLeft.x, bottomLeft.y, bottomLeft.z);
    cubos::logDebug("{},{},{}", bottomRight.x, bottomRight.y, bottomRight.z);
    cubos::logDebug("{},{},{}", topLeft.x, topLeft.y, topLeft.z);
    cubos::logDebug("{},{},{}", topRight.x, topRight.y, topRight.z);

    Triangle triangle_bl, triangle_tr;

    triangle_bl.v0.material = triangle_bl.v1.material = triangle_bl.v2.material = triangle_tr.v0.material =
        triangle_tr.v1.material = triangle_tr.v2.material = material_id;

    auto normal = glm::cross(topLeft - bottomLeft, bottomRight - bottomLeft);

    triangle_bl.v0.normal = triangle_bl.v1.normal = triangle_bl.v2.normal = triangle_tr.v0.normal =
        triangle_tr.v1.normal = triangle_tr.v2.normal = normal;

    triangle_bl.v0.position = topLeft;
    triangle_bl.v1.position = bottomRight;
    triangle_bl.v2.position = bottomLeft;

    triangle_tr.v0.position = topRight;
    triangle_tr.v1.position = bottomRight;
    triangle_tr.v2.position = topLeft;

    triangles.push_back(triangle_bl);
    triangles.push_back(triangle_tr);
}

vector<Triangle> Triangulation::Triangulate(const Grid& grid)
{
    glm::uvec3 grid_size = grid.getSize();
    vector<Triangle> triangles;
    std::vector<uint16_t> mask;

    glm::vec3 offset = -glm::vec3(grid_size) / 2.0f;

    // For both back and front faces
    bool back_face = true;
    do
    {
        back_face = !back_face;

        // For each axis
        for (int d = 0; d < 3; ++d)
        {
            int u = (d + 1) % 3;
            int v = (d + 2) % 3;

            glm::ivec3 x = {0, 0, 0}, q = {0, 0, 0};
            q[d] = 1;
            mask.resize(grid_size[u] * grid_size[v]);

            for (x[d] = -1; x[d] < int(grid_size[d]);)
            {
                int n = 0;

                // Create mask
                for (x[v] = 0; x[v] < int(grid_size[v]); ++x[v])
                {
                    for (x[u] = 0; x[u] < int(grid_size[u]); ++x[u])
                    {
                        if (x[d] < 0)
                        {
                            mask[n++] = back_face ? grid.get({x.x + q.x, x.y + q.y, x.z + q.z}) : 0;
                        }
                        else if (x[d] == int(grid_size[d]) - 1)
                        {
                            mask[n++] = back_face ? 0 : grid.get({x.x, x.y, x.z});
                        }
                        else if (grid.get({x.x, x.y, x.z}) == 0 || grid.get({x.x + q.x, x.y + q.y, x.z + q.z}) == 0)
                        {
                            mask[n++] =
                                back_face ? grid.get({x.x + q.x, x.y + q.y, x.z + q.z}) : grid.get({x.x, x.y, x.z});
                        }
                        else
                        {
                            mask[n++] = 0;
                        }
                    }
                }

                ++x[d];
                n = 0;

                // Generate mesh from mask
                for (int j = 0; j < int(grid_size[v]); ++j)
                {
                    for (int i = 0; i < int(grid_size[u]);)
                    {
                        if (mask[n] != 0)
                        {
                            int w, h;
                            for (w = 1; i + w < int(grid_size[u]) && mask[n + w] == mask[n]; ++w)
                                ;
                            bool done = false;
                            for (h = 1; j + h < int(grid_size[v]); ++h)
                            {
                                for (int k = 0; k < w; ++k)
                                {
                                    if (mask[n + k + h * grid_size[u]] == 0 ||
                                        mask[n + k + h * grid_size[u]] != mask[n])
                                    {
                                        done = true;
                                        break;
                                    }
                                }

                                if (done)
                                {
                                    break;
                                }
                            }

                            if (mask[n] != 0)
                            {
                                x[u] = i;
                                x[v] = j;

                                glm::ivec3 du = {0, 0, 0}, dv = {0, 0, 0};
                                du[u] = w;
                                dv[v] = h;

                                if (!back_face)
                                {
                                    add_triangle_from_quad(triangles, offset + glm::vec3(x + du),
                                                           offset + glm::vec3(x + du + dv), offset + glm::vec3(x),
                                                           offset + glm::vec3(x + dv), mask[n] - 1);
                                }
                                else
                                {
                                    add_triangle_from_quad(triangles, offset + glm::vec3(x + du + dv),
                                                           offset + glm::vec3(x + du), offset + glm::vec3(x + dv),
                                                           offset + glm::vec3(x), mask[n] - 1);
                                }
                            }

                            for (int l = 0; l < h; ++l)
                            {
                                for (int k = 0; k < w; ++k)
                                {
                                    mask[n + k + l * grid_size[u]] = 0;
                                }
                            }

                            i += w;
                            n += w;
                        }
                        else
                        {
                            ++i;
                            ++n;
                        }
                    }
                }
            }
        }
    } while (back_face != true);
    for (auto it = triangles.begin(); it != triangles.end(); it++)
    {
        logDebug("VO:");
        logDebug("pos: {},{},{}", it->v0.position.x, it->v0.position.y, it->v0.position.z);
        logDebug("normal: {},{},{}", it->v0.normal.x, it->v0.normal.y, it->v0.normal.z);
        logDebug("material: {}", it->v0.material);

        logDebug("V1:");
        logDebug("pos: {},{},{}", it->v1.position.x, it->v1.position.y, it->v1.position.z);
        logDebug("normal: {},{},{}", it->v1.normal.x, it->v1.normal.y, it->v1.normal.z);
        logDebug("material: {}", it->v1.material);

        logDebug("V2:");
        logDebug("pos: {},{},{}", it->v2.position.x, it->v2.position.y, it->v2.position.z);
        logDebug("normal: {},{},{}", it->v2.normal.x, it->v2.normal.y, it->v2.normal.z);
        logDebug("material: {}", it->v2.material);
    }

    return triangles;
}
