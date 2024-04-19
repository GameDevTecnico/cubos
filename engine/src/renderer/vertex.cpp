#include <vector>

#include <cubos/engine/renderer/vertex.hpp>
#include <cubos/engine/voxels/grid.hpp>

using namespace cubos::engine;

void cubos::engine::triangulate(const VoxelGrid& grid, std::vector<VoxelVertex>& vertices,
                                std::vector<uint32_t>& indices)
{
    std::vector<uint16_t> mask;

    const auto& sz = grid.size();

    // For both back and front faces.
    bool backFace = true;
    do
    {
        backFace = !backFace;

        // For each axis.
        for (int d = 0; d < 3; ++d)
        {
            int u = (d + 1) % 3;
            int v = (d + 2) % 3;

            glm::ivec3 x = {0, 0, 0};
            glm::ivec3 q = {0, 0, 0};
            q[d] = 1;
            mask.resize(static_cast<std::size_t>(sz[u]) * static_cast<std::size_t>(sz[v]));

            for (x[d] = -1; x[d] < int(sz[d]);)
            {
                std::size_t n = 0;

                // Create mask
                for (x[v] = 0; x[v] < int(sz[v]); ++x[v])
                {
                    for (x[u] = 0; x[u] < int(sz[u]); ++x[u])
                    {
                        if (x[d] < 0)
                        {
                            mask[n++] = backFace ? grid.get(x + q) : 0;
                        }
                        else if (x[d] == int(sz[d]) - 1)
                        {
                            mask[n++] = backFace ? 0 : grid.get(x);
                        }
                        else if (grid.get(x) == 0 || grid.get(x + q) == 0)
                        {
                            mask[n++] = backFace ? grid.get(x + q) : grid.get(x);
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
                for (std::size_t j = 0; j < sz[v]; ++j)
                {
                    for (std::size_t i = 0; i < sz[u];)
                    {
                        if (mask[n] != 0)
                        {
                            std::size_t w;
                            std::size_t h;
                            for (w = 1; i + w < sz[u] && mask[n + w] == mask[n]; ++w)
                            {
                                ;
                            }
                            bool done = false;
                            for (h = 1; j + h < sz[v]; ++h)
                            {
                                for (std::size_t k = 0; k < w; ++k)
                                {
                                    if (mask[n + k + h * sz[u]] == 0 || mask[n + k + h * sz[u]] != mask[n])
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
                                x[u] = static_cast<int>(i);
                                x[v] = static_cast<int>(j);

                                glm::ivec3 du = {0, 0, 0};
                                glm::ivec3 dv = {0, 0, 0};
                                du[u] = static_cast<int>(w);
                                dv[v] = static_cast<int>(h);

                                auto vi = vertices.size();
                                vertices.resize(vi + 4, {{}, backFace ? -q : q, mask[n]});
                                vertices[vi + 0].position = x;
                                vertices[vi + 1].position = x + du;
                                vertices[vi + 2].position = x + du + dv;
                                vertices[vi + 3].position = x + dv;

                                auto ii = indices.size();
                                indices.resize(ii + 6);
                                if (backFace)
                                {
                                    indices[ii + 0] = static_cast<uint32_t>(vi) + 0;
                                    indices[ii + 1] = static_cast<uint32_t>(vi) + 2;
                                    indices[ii + 2] = static_cast<uint32_t>(vi) + 1;
                                    indices[ii + 3] = static_cast<uint32_t>(vi) + 3;
                                    indices[ii + 4] = static_cast<uint32_t>(vi) + 2;
                                    indices[ii + 5] = static_cast<uint32_t>(vi) + 0;
                                }
                                else
                                {
                                    indices[ii + 0] = static_cast<uint32_t>(vi) + 0;
                                    indices[ii + 1] = static_cast<uint32_t>(vi) + 1;
                                    indices[ii + 2] = static_cast<uint32_t>(vi) + 2;
                                    indices[ii + 3] = static_cast<uint32_t>(vi) + 2;
                                    indices[ii + 4] = static_cast<uint32_t>(vi) + 3;
                                    indices[ii + 5] = static_cast<uint32_t>(vi) + 0;
                                }
                            }

                            for (std::size_t l = 0; l < h; ++l)
                            {
                                for (std::size_t k = 0; k < w; ++k)
                                {
                                    mask[n + k + l * sz[u]] = 0;
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
    } while (!backFace);
}
