#include <cubos/gl/vertex.hpp>
#include <cubos/gl/grid.hpp>

#include <vector>

using namespace cubos;
using namespace cubos::gl;

void Vertex::serialize(memory::Serializer& serializer) const
{
    serializer.write(this->position, "position");
    serializer.write(this->normal, "normal");
    serializer.write(this->material, "material");
}

void Vertex::deserialize(memory::Deserializer& deserializer)
{
    deserializer.read(this->position);
    deserializer.read(this->normal);
    deserializer.read(this->material);
}

void cubos::gl::triangulate(const Grid& grid, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    std::vector<uint16_t> mask;

    auto& sz = grid.getSize();

    // For both back and front faces.
    bool back_face = true;
    do
    {
        back_face = !back_face;

        // For each axis.
        for (int d = 0; d < 3; ++d)
        {
            int u = (d + 1) % 3;
            int v = (d + 2) % 3;

            glm::ivec3 x = {0, 0, 0}, q = {0, 0, 0};
            q[d] = 1;
            mask.resize(sz[u] * sz[v]);

            for (x[d] = -1; x[d] < int(sz[d]);)
            {
                int n = 0;

                // Create mask
                for (x[v] = 0; x[v] < int(sz[v]); ++x[v])
                    for (x[u] = 0; x[u] < int(sz[u]); ++x[u])
                    {
                        if (x[d] < 0)
                            mask[n++] = back_face ? grid.get(x + q) : 0;
                        else if (x[d] == int(sz[d]) - 1)
                            mask[n++] = back_face ? 0 : grid.get(x);
                        else if (grid.get(x) == 0 || grid.get(x + q) == 0)
                            mask[n++] = back_face ? grid.get(x + q) : grid.get(x);
                        else
                            mask[n++] = 0;
                    }

                ++x[d];
                n = 0;

                // Generate mesh from mask
                for (int j = 0; j < int(sz[v]); ++j)
                {
                    for (int i = 0; i < int(sz[u]);)
                    {
                        if (mask[n] != 0)
                        {
                            int w, h;
                            for (w = 1; i + w < int(sz[u]) && mask[n + w] == mask[n]; ++w)
                                ;
                            bool done = false;
                            for (h = 1; j + h < int(sz[v]); ++h)
                            {
                                for (int k = 0; k < w; ++k)
                                    if (mask[n + k + h * sz[u]] == 0 || mask[n + k + h * sz[u]] != mask[n])
                                    {
                                        done = true;
                                        break;
                                    }

                                if (done)
                                    break;
                            }

                            if (mask[n] != 0)
                            {
                                x[u] = i;
                                x[v] = j;

                                glm::ivec3 du = {0, 0, 0}, dv = {0, 0, 0};
                                du[u] = w;
                                dv[v] = h;

                                auto vi = vertices.size();
                                vertices.resize(vi + 4, {{}, back_face ? -q : q, mask[n]});
                                vertices[vi + 0].position = x;
                                vertices[vi + 1].position = x + du;
                                vertices[vi + 2].position = x + du + dv;
                                vertices[vi + 3].position = x + dv;

                                auto ii = indices.size();
                                indices.resize(ii + 6);
                                if (back_face)
                                {
                                    indices[ii + 0] = int(vi) + 0;
                                    indices[ii + 1] = int(vi) + 2;
                                    indices[ii + 2] = int(vi) + 1;
                                    indices[ii + 3] = int(vi) + 3;
                                    indices[ii + 4] = int(vi) + 2;
                                    indices[ii + 5] = int(vi) + 0;
                                }
                                else
                                {
                                    indices[ii + 0] = int(vi) + 0;
                                    indices[ii + 1] = int(vi) + 1;
                                    indices[ii + 2] = int(vi) + 2;
                                    indices[ii + 3] = int(vi) + 2;
                                    indices[ii + 4] = int(vi) + 3;
                                    indices[ii + 5] = int(vi) + 0;
                                }
                            }

                            for (int l = 0; l < h; ++l)
                                for (int k = 0; k < w; ++k)
                                    mask[n + k + l * sz[u]] = 0;

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
}
