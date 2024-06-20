#include <cstddef>

#include <cubos/core/tel/logging.hpp>

#include <cubos/engine/render/mesh/vertex.hpp>
#include <cubos/engine/voxels/grid.hpp>

using cubos::core::gl::Type;
using cubos::core::gl::VertexElement;
using cubos::engine::RenderMeshVertex;

bool RenderMeshVertex::addVertexElements(const char* position, const char* normal, const char* material,
                                         std::size_t bufferIndex, core::gl::VertexArrayDesc& desc)
{
    if (desc.elementCount + 3 > CUBOS_CORE_GL_MAX_VERTEX_ARRAY_ELEMENT_COUNT)
    {
        CUBOS_ERROR("Couldn't add vertex elements to vertex array description, as it's already full");
        return false;
    }

    desc.elements[desc.elementCount++] = {
        .name = position,
        .type = Type::UByte,
        .size = 3,
        .buffer = {.stride = sizeof(RenderMeshVertex),
                   .offset = offsetof(RenderMeshVertex, position),
                   .index = bufferIndex},
    };

    desc.elements[desc.elementCount++] = {
        .name = normal,
        .type = Type::UByte,
        .size = 1,
        .buffer = {.stride = sizeof(RenderMeshVertex),
                   .offset = offsetof(RenderMeshVertex, normal),
                   .index = bufferIndex},
    };

    desc.elements[desc.elementCount++] = {
        .name = material,
        .type = Type::UInt,
        .size = 1,
        .buffer = {.stride = sizeof(RenderMeshVertex),
                   .offset = offsetof(RenderMeshVertex, material),
                   .index = bufferIndex},
    };

    return true;
}

void RenderMeshVertex::generate(const VoxelGrid& grid, std::vector<RenderMeshVertex>& vertices)
{
    std::vector<uint32_t> mask;

    const auto& sz = grid.size();
    CUBOS_ASSERT(sz.x <= 255 && sz.y <= 255 && sz.z <= 255,
                 "Only grids with sizes up to 255 in each dimension are supported");

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

            for (x[d] = -1; x[d] < static_cast<int>(sz[d]);)
            {
                std::size_t n = 0;

                // Create mask
                for (x[v] = 0; x[v] < static_cast<int>(sz[v]); ++x[v])
                {
                    for (x[u] = 0; x[u] < static_cast<int>(sz[u]); ++x[u])
                    {
                        if (x[d] < 0)
                        {
                            mask[n++] = backFace ? grid.get(x + q) : 0;
                        }
                        else if (x[d] == static_cast<uint8_t>(sz[d] - 1))
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
                                x[u] = static_cast<uint8_t>(i);
                                x[v] = static_cast<uint8_t>(j);

                                glm::u8vec3 du = {0, 0, 0};
                                glm::u8vec3 dv = {0, 0, 0};
                                du[u] = static_cast<uint8_t>(w);
                                dv[v] = static_cast<uint8_t>(h);

                                uint8_t normal;
                                switch (d)
                                {
                                case 0:
                                    normal = backFace ? 1 : 0;
                                    break;
                                case 1:
                                    normal = backFace ? 3 : 2;
                                    break;
                                case 2:
                                    normal = backFace ? 5 : 4;
                                    break;
                                default:
                                    CUBOS_UNREACHABLE();
                                }

                                auto vi = vertices.size();
                                vertices.resize(vi + 6, RenderMeshVertex{
                                                            .position = {},
                                                            .normal = normal,
                                                            .material = static_cast<uint32_t>(mask[n]),
                                                        });

                                vertices[vi + 0].position = static_cast<glm::u8vec3>(x);
                                vertices[vi + 1].position = static_cast<glm::u8vec3>(x) + du;
                                vertices[vi + 2].position = static_cast<glm::u8vec3>(x) + du + dv;
                                vertices[vi + 3].position = static_cast<glm::u8vec3>(x) + du + dv;
                                vertices[vi + 4].position = static_cast<glm::u8vec3>(x) + dv;
                                vertices[vi + 5].position = static_cast<glm::u8vec3>(x);

                                if (backFace)
                                {
                                    std::swap(vertices[vi + 1].position, vertices[vi + 2].position);
                                    std::swap(vertices[vi + 3].position, vertices[vi + 4].position);
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
