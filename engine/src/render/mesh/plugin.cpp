#include <unordered_map>
#include <unordered_set>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/thread/pool.hpp>
#include <cubos/core/thread/task.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/mesh/mesh.hpp>
#include <cubos/engine/render/mesh/plugin.hpp>
#include <cubos/engine/render/mesh/pool.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::Window;
using cubos::core::thread::Task;
using cubos::core::thread::ThreadPool;
using cubos::engine::Asset;
using cubos::engine::RenderMeshPool;
using cubos::engine::RenderMeshVertex;
using cubos::engine::VoxelGrid;

CUBOS_DEFINE_TAG(cubos::engine::renderMeshPoolInitTag);

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        struct Entry
        {
            RenderMeshPool::BucketId firstBucketId{};
            Asset<VoxelGrid> asset{};
            int referenceCount{};
            Task<std::vector<RenderMeshVertex>> meshingTask{};
        };

        ThreadPool pool;
        std::unordered_map<uuids::uuid, Entry> entries;
        std::unordered_set<uuids::uuid> meshing;

        State(std::size_t threadCount)
            : pool(threadCount)
        {
        }
    };
} // namespace

void cubos::engine::renderMeshPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);
    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(renderVoxelsPlugin);

    cubos.uninitResource<RenderMeshPool>();
    cubos.uninitResource<State>();

    cubos.component<RenderMesh>();

    cubos.startupTag(renderMeshPoolInitTag).after(windowInitTag).after(settingsTag);

    cubos.startupSystem("setup RenderMeshPool and meshing threads")
        .tagged(renderMeshPoolInitTag)
        .call([](Commands cmds, const Window& window, Settings& settings) {
            cmds.emplaceResource<RenderMeshPool>(
                window->renderDevice(),
                static_cast<std::size_t>(settings.getInteger("renderMeshPool.bucketCount", 1024)),
                static_cast<std::size_t>(settings.getInteger("renderMeshPool.bucketSize", 1024 * 6)));

            cmds.emplaceResource<State>(static_cast<std::size_t>(settings.getInteger("renderMeshPool.threadCount", 1)));
        });

    cubos.observer("update RenderMesh on LoadRenderVoxels")
        .onAdd<LoadRenderVoxels>()
        .call([](Commands cmds, State& state, Assets& assets, Query<Entity, RenderVoxelGrid&> query) {
            for (auto [ent, grid] : query)
            {
                if (grid.asset.isNull())
                {
                    continue;
                }

                // The asset handle might not yet have a valid ID, and since we need getId, we load it first.
                grid.asset = assets.load(grid.asset);
                // If the asset has never been loaded, or it has been updated since the last meshing, update the entry
                // and queue a meshing task. Otherwise, just reuse the existing mesh.
                auto& entry = state.entries[grid.asset.getId().value()];
                entry.referenceCount += 1;
                if (entry.asset.isNull() || assets.update(entry.asset))
                {
                    entry.firstBucketId = {};
                    entry.asset = grid.asset;
                    assets.update(entry.asset); // Make sure the stored handle is up-to-date.

                    // Spawn a thread task to mesh the voxels
                    entry.meshingTask = {}; // Reset the task.
                    state.pool.addTask([&assets, task = entry.meshingTask, asset = grid.asset]() mutable {
                        // Load the voxel data.
                        auto grid = assets.read<VoxelGrid>(asset);

                        // Generate a mesh from the voxel data.
                        std::vector<RenderMeshVertex> vertices{};
                        RenderMeshVertex::generate(*grid, vertices);

                        // Output the task result.
                        task.finish(std::move(vertices));
                    });

                    state.meshing.insert(grid.asset.getId().value());
                }
                else if (entry.firstBucketId != RenderMeshPool::BucketId::Invalid)
                {
                    auto assetRead = assets.read(entry.asset);
                    cubos::core::geom::Box box{.halfSize = glm::vec3(assetRead->size()) * 0.5F};
                    cmds.add(ent, RenderMesh{.firstBucketId = entry.firstBucketId,
                                             .baseOffset = -static_cast<glm::vec3>(assetRead->size()) / 2.0F,
                                             .boundingBox = box});
                    cmds.remove<LoadRenderVoxels>(ent);
                }
            }
        });

    cubos.system("poll RenderMesh tasks")
        .call([](Commands cmds, Assets& assets, State& state,
                 Query<Entity, RenderVoxelGrid&, const LoadRenderVoxels&> query, RenderMeshPool& pool) {
            for (auto [ent, grid, load] : query)
            {
                if (grid.asset.isNull())
                {
                    cmds.remove<LoadRenderVoxels>(ent);
                    continue;
                }

                auto& entry = state.entries[grid.asset.getId().value()];
                if (!state.meshing.contains(grid.asset.getId().value()))
                {
                    if (entry.asset.isNull() || assets.update(entry.asset))
                    {
                        entry.firstBucketId = {};
                        entry.asset = grid.asset;
                        assets.update(entry.asset); // Make sure the stored handle is up-to-date.

                        // Spawn a thread task to mesh the voxels
                        entry.meshingTask = {}; // Reset the task.
                        state.pool.addTask([&assets, task = entry.meshingTask, asset = grid.asset]() mutable {
                            // Load the voxel data.
                            auto grid = assets.read<VoxelGrid>(asset);

                            // Generate a mesh from the voxel data.
                            std::vector<RenderMeshVertex> vertices{};
                            RenderMeshVertex::generate(*grid, vertices);

                            // Output the task result.
                            task.finish(std::move(vertices));
                        });

                        state.meshing.insert(grid.asset.getId().value());
                    }
                    else
                    {
                        CUBOS_ASSERT(entry.firstBucketId != RenderMeshPool::BucketId::Invalid);
                        auto assetRead = assets.read(entry.asset);
                        cubos::core::geom::Box box{.halfSize = glm::vec3(assetRead->size()) * 0.5F};
                        cmds.add(ent, RenderMesh{.firstBucketId = entry.firstBucketId,
                                                 .baseOffset = -static_cast<glm::vec3>(assetRead->size()) / 2.0F,
                                                 .boundingBox = box});
                        cmds.remove<LoadRenderVoxels>(ent);
                    }
                }
                else if (entry.meshingTask.isDone())
                {
                    auto vertices = entry.meshingTask.result();
                    auto firstBucketId = pool.allocate(vertices.data(), vertices.size());
                    cmds.remove<LoadRenderVoxels>(ent);
                    auto assetRead = assets.read(entry.asset);
                    cubos::core::geom::Box box{.halfSize = glm::vec3(assetRead->size()) * 0.5F};
                    cmds.add(ent, RenderMesh{.firstBucketId = firstBucketId,
                                             .baseOffset = -static_cast<glm::vec3>(assetRead->size()) / 2.0F,
                                             .boundingBox = box});

                    CUBOS_INFO("Finished meshing voxel grid asset {} ({} vertices), allocated new render bucket",
                               grid.asset.getIdString(), vertices.size());

                    auto& entry = state.entries[grid.asset.getId().value()];
                    if (entry.firstBucketId != RenderMeshPool::BucketId::Invalid)
                    {
                        pool.deallocate(entry.firstBucketId);
                        CUBOS_INFO("Deallocated old render mesh bucket of asset {}", grid.asset.getIdString());
                    }
                    entry.firstBucketId = firstBucketId;
                    state.meshing.erase(grid.asset.getId().value());
                }
            }
        });

    cubos.observer("decrease reference count of removed RenderMeshes")
        .onRemove<RenderMesh>()
        .call([](Query<Entity, const RenderVoxelGrid&> query, RenderMeshPool& pool, State& state) {
            for (auto [ent, grid] : query)
            {
                auto& entry = state.entries[grid.asset.getId().value()];
                entry.referenceCount -= 1;
                if (entry.referenceCount == 0)
                {
                    pool.deallocate(entry.firstBucketId);
                    state.entries.erase(grid.asset.getId().value());
                    CUBOS_INFO("Deallocated render mesh bucket of asset {} (no more references)",
                               grid.asset.getIdString());
                }
            }
        });
}
