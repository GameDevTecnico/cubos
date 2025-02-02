#include "interface/plugin.hpp"
#include <cstdint>

#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/shapes/capsule.hpp>
#include <cubos/engine/collisions/shapes/voxel.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>

#include "broad_phase/plugin.hpp"
#include "narrow_phase/plugin.hpp"

CUBOS_DEFINE_TAG(cubos::engine::collisionsTag);

using namespace std;

inline void voxelGridToAABB(const cubos::engine::VoxelGrid& grid, cubos::core::geom::AABB& aabb)
{
    glm::ivec3 size = grid.size();
    glm::ivec3 min = glm::ivec3({size.x - 1, size.y - 1, size.z - 1});
    glm::ivec3 max = glm::ivec3({0, 0, 0});

    for (int x = 0; x < size.x; x++)
    {
        for (int y = 0; y < size.y; y++)
        {
            for (int z = 0; z < size.z; z++)
            {
                if (grid.get({x, y, z}) != 0U)
                {
                    min = glm::min(min, glm::ivec3({x, y, z}));
                    max = glm::max(max, glm::ivec3({x, y, z}));
                }
            }
        }
    }
    max = glm::ivec3(max.x + 1, max.y + 1, max.z + 1);
    glm::vec3 dist = glm::vec3(max) - glm::vec3(min);
    aabb.min({-dist.x / 2.0F, -dist.y / 2.0F, -dist.z / 2.0F});
    aabb.max({dist.x / 2.0F, dist.y / 2.0F, dist.z / 2.0F});
}

static pair<glm::uvec3, glm::uvec3> findLargestBox(const cubos::engine::VoxelGrid& grid,
                                                   vector<vector<vector<bool>>>& processed, const glm::uvec3& start)
{
    glm::uvec3 gridSize = grid.size();
    glm::uvec3 end = start;

    // Expand X axis
    while (end.x + 1 < gridSize.x)
    {
        bool canExpand = true;
        for (std::uint32_t y = start.y; y <= end.y; y++)
        {
            for (std::uint32_t z = start.z; z <= end.z; z++)
            {
                // Convert to ivec3 when calling grid.get
                glm::ivec3 pos(static_cast<int>(end.x) + 1, static_cast<int>(y), static_cast<int>(z));
                if (grid.get(pos) == 0U || processed.at(static_cast<unsigned long>(end.x) + 1).at(y).at(z))
                {
                    canExpand = false;
                    break;
                }
            }
            if (!canExpand)
            {
                break; // Break outer loop if not expandable
            }
        }
        if (canExpand)
        {
            end.x++;
        }
        else
        {
            break; // Exit while loop if cannot expand
        }
    }

    // Expand Y axis
    while (end.y + 1 < gridSize.y)
    {
        bool canExpand = true;
        for (std::uint32_t x = start.x; x <= end.x; x++)
        {
            for (std::uint32_t z = start.z; z <= end.z; z++)
            {
                glm::ivec3 pos(static_cast<int>(x), static_cast<int>(end.y) + 1, static_cast<int>(z));
                if (grid.get(pos) == 0U || processed.at(x).at(static_cast<unsigned long>(end.y) + 1).at(z))
                {
                    canExpand = false;
                    break;
                }
            }
            if (!canExpand)
            {
                break; // Break outer loop if not expandable
            }
        }
        if (canExpand)
        {
            end.y++;
        }
        else
        {
            break; // Exit while loop if cannot expand
        }
    }

    // Expand Z axis
    while (end.z + 1 < gridSize.z)
    {
        bool canExpand = true;
        for (std::uint32_t x = start.x; x <= end.x; x++)
        {
            for (std::uint32_t y = start.y; y <= end.y; y++)
            {
                glm::ivec3 pos(static_cast<int>(x), static_cast<int>(y), static_cast<int>(end.z) + 1);
                if (grid.get(pos) == 0U || processed.at(x).at(y).at(static_cast<unsigned long>(end.z) + 1))
                {
                    canExpand = false;
                    break;
                }
            }
            if (!canExpand)
            {
                break; // Break outer loop if not expandable
            }
        }
        if (canExpand)
        {
            end.z++;
        }
        else
        {
            break; // Exit while loop if cannot expand
        }
    }

    // Mark voxels within the box as processed
    for (std::uint32_t x = start.x; x <= end.x; x++)
    {
        for (std::uint32_t y = start.y; y <= end.y; y++)
        {
            for (std::uint32_t z = start.z; z <= end.z; z++)
            {
                processed[x][y][z] = true;
            }
        }
    }

    return std::make_pair(start, end);
}

static vector<pair<glm::uvec3, glm::uvec3>> greedy3dMeshing(const cubos::engine::VoxelGrid& grid)
{
    const glm::uvec3 gridSize = grid.size();

    // Vector of processed voxels
    vector<vector<vector<bool>>> processed(gridSize.x,
                                           vector<vector<bool>>(gridSize.y, vector<bool>(gridSize.z, false)));

    // Vector with min and max of boxes found
    vector<pair<glm::uvec3, glm::uvec3>> boxes;

    for (std::uint32_t x = 0; x < gridSize.x; x++)
    {
        for (std::uint32_t y = 0; y < gridSize.y; y++)
        {
            for (std::uint32_t z = 0; z < gridSize.z; z++)
            {
                if (grid.get({x, y, z}) != 0U && !processed[x][y][z])
                {
                    boxes.push_back(findLargestBox(grid, processed, {x, y, z}));
                }
            }
        }
    }
    return boxes;
}

void cubos::engine::collisionsPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(assetsPlugin);

    cubos.plugin(interfaceCollisionsPlugin);
    cubos.plugin(broadPhaseCollisionsPlugin);
    cubos.plugin(narrowPhaseCollisionsPlugin);

    cubos.tag(collisionsTag).addTo(collisionsBroadTag).addTo(collisionsNarrowTag).addTo(collisionsManifoldTag);

    auto initializeBoxColliders = [](Query<const BoxCollisionShape&, Collider&> query) {
        for (auto [shape, collider] : query)
        {
            shape.box.diag(collider.localAABB.diag);
            collider.margin = 0.04F;
        }
    };

    cubos.observer("setup Box Colliders").onAdd<BoxCollisionShape>().call(initializeBoxColliders);
    cubos.observer("setup Box Colliders").onAdd<Collider>().call(initializeBoxColliders);

    auto initializeCapsuleColliders = [](Query<const CapsuleCollisionShape&, Collider&> query) {
        for (auto [shape, collider] : query)
        {
            collider.localAABB = shape.capsule.aabb();
            collider.margin = 0.0F;
        }
    };

    cubos.observer("setup Capsule Colliders").onAdd<CapsuleCollisionShape>().call(initializeCapsuleColliders);
    cubos.observer("setup Capsule Colliders").onAdd<Collider>().call(initializeCapsuleColliders);

    auto initializeVoxelColliders = [](const Assets& assets, Query<VoxelCollisionShape&, Collider&> query) {
        for (auto [shape, collider] : query)
        {
            // load the voxel grid
            const VoxelGrid& grid = assets.read(shape.grid).get();
            // set the local AABB
            voxelGridToAABB(grid, collider.localAABB);
            collider.margin = 0.0F;

            glm::uvec3 size = grid.size();
            glm::vec3 center = glm::vec3(size) / 2.0F;
            auto boxCoords = greedy3dMeshing(grid);
            uint32_t boxId = 1;

            for (auto corners : boxCoords)
            {
                cubos::core::geom::Box box;
                auto boxCenter = glm::vec3(glm::ivec3(corners.first + corners.second) + glm::ivec3(1, 1, 1)) / 2.0F;
                glm::vec3 shift = center - boxCenter;
                box.halfSize = glm::vec3(glm::ivec3(corners.second - corners.first) + glm::ivec3(1, 1, 1)) / 2.0F;
                shape.insertBox(box, shift, boxId);
                boxId++;
            }
        }
    };

    cubos.observer("setup Voxel Colliders").onAdd<VoxelCollisionShape>().call(initializeVoxelColliders);
    cubos.observer("setup Voxel Colliders").onAdd<Collider>().call(initializeVoxelColliders);
}
