#include "interface/plugin.hpp"

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

void expandBoxZaxis(cubos::engine::VoxelGrid& grid, const glm::uvec3& currentMin, const glm::uvec3& maxSize,
                    glm::uvec3& possibleSize);
void expandBoxYaxis(cubos::engine::VoxelGrid& grid, const glm::uvec3& currentMin, const glm::uvec3& maxSize,
                    glm::uvec3& possibleSize);

inline void voxelGridToAABB(const cubos::engine::VoxelGrid& grid, cubos::core::geom::AABB& aabb)
{
    glm::uvec3 min = grid.size();
    glm::uvec3 max = glm::uvec3(0);
    for (uint32_t x = min.x; x < max.x; x++)
    {
        for (uint32_t y = min.y; y < max.y; y++)
        {
            for (uint32_t z = min.z; z < max.z; z++)
            {
                if (grid.get({x, y, z}) != 0)
                {
                    auto currentCoords = glm::uvec3({x, y, z});
                    min = glm::min(min, currentCoords);
                    max = glm::max(max, currentCoords);
                }
            }
        }
    }
    aabb.min(-glm::ivec3({static_cast<int>(min.x / 2), static_cast<int>(min.y / 2), static_cast<int>(min.z / 2)}));
    aabb.max(glm::ivec3({static_cast<int>(min.x / 2), static_cast<int>(min.y / 2), static_cast<int>(min.z / 2)}));
}

void expandBoxYaxis(cubos::engine::VoxelGrid& grid, const glm::uvec3& currentMin, const glm::uvec3& maxSize,
                    glm::uvec3& possibleSize)
{
    possibleSize.y = 1;
    for (size_t y = currentMin.y + 1; y < maxSize.y; y++)
    {
        bool canExpand = true;
        for (size_t x = currentMin.x; x < std::min(maxSize.x, (currentMin.x + possibleSize.x)); x++)
        {
            if (grid.get({x, y, currentMin.z}) == 0)
            {
                canExpand = false;
                break;
            }
        }
        if (canExpand)
        {
            possibleSize.y++;
            for (size_t x = currentMin.x; x < std::min(maxSize.x, (currentMin.x + possibleSize.x)); x++)
            {
                grid.set({x, y, currentMin.z}, 0);
            }
            if (y < maxSize.y - 1)
            {
                continue;
            }
        }
        expandBoxZaxis(grid, currentMin, maxSize, possibleSize);
        break;
    }
}

void expandBoxZaxis(cubos::engine::VoxelGrid& grid, const glm::uvec3& currentMin, const glm::uvec3& maxSize,
                    glm::uvec3& possibleSize)
{
    possibleSize.z = 1;
    for (size_t z = currentMin.z + 1; z < maxSize.z; z++)
    {
        bool canExpand = true;
        for (size_t y = currentMin.y; y < std::min(maxSize.y, (currentMin.y + possibleSize.y)); y++)
        {
            for (size_t x = currentMin.x; x < std::min(maxSize.x, (currentMin.x + possibleSize.x)); x++)
            {
                if (grid.get({x, y, z}) == 0)
                {
                    canExpand = false;
                    break;
                }
            }
        }
        if (canExpand)
        {
            possibleSize.z++;
            for (size_t z = currentMin.z + 1; z < std::min(maxSize.z, (currentMin.z + possibleSize.z)); z++)
            {
                for (size_t y = currentMin.y; y < std::min(maxSize.y, (currentMin.y + possibleSize.y)); y++)
                {
                    for (size_t x = currentMin.x; x < std::min(maxSize.x, (currentMin.x + possibleSize.x)); x++)
                    {
                        grid.set({x, y, z}, 0);
                    }
                }
            }
        }
        break;
    }
}

void cubos::engine::collisionsPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(assetsPlugin);

    cubos.plugin(interfaceCollisionsPlugin);
    cubos.plugin(broadPhaseCollisionsPlugin);
    cubos.plugin(narrowPhaseCollisionsPlugin);

    cubos.tag(collisionsTag).addTo(collisionsBroadTag).addTo(collisionsNarrowTag);

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

            // decompose the voxel grid into a list of boxes
            VoxelGrid copiedGrid = VoxelGrid();
            copiedGrid = grid;
            glm::uvec3 size = grid.size();
            glm::ivec3 currentMin = glm::ivec3(0);
            glm::vec3 center = glm::vec3(size) / 2.0F;
            glm::ivec3 currentCoords;
            glm::uvec3 possibleSize = glm::uvec3(0);
            for (uint32_t z = 0; z < size.z; z++)
            {
                for (uint32_t y = 0; y < size.y; y++)
                {
                    for (uint32_t x = 0; x < size.x; x++)
                    {
                        currentCoords = glm::ivec3({x, y, z});
                        if (copiedGrid.get(currentCoords) != 0)
                        {
                            possibleSize.x++;
                            copiedGrid.set(currentCoords, 0);
                            if (x < size.x - 1)
                            {
                                continue;
                            }
                        }
                        if (possibleSize.x > 0)
                        {
                            expandBoxYaxis(copiedGrid, currentMin, size, possibleSize);
                            glm::vec3 boxCenter = glm::vec3(currentMin + static_cast<glm::ivec3>(possibleSize)) / 2.0F;
                            cubos::core::geom::Box box;
                            box.halfSize = boxCenter;
                            glm::vec3 shift = center - boxCenter;
                            shape.insertBox(box, shift);
                            currentMin = currentCoords;
                            possibleSize = glm::uvec3(0);
                        }
                    }
                }
            }
        }
    };

    cubos.observer("setup Voxel Colliders").onAdd<VoxelCollisionShape>().call(initializeVoxelColliders);
    cubos.observer("setup Voxel Colliders").onAdd<Collider>().call(initializeVoxelColliders);
}
