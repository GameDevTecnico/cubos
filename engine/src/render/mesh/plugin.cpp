#include <unordered_map>

#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/uuid.hpp>
#include <cubos/core/thread/pool.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/mesh/mesh.hpp>
#include <cubos/engine/render/mesh/plugin.hpp>
#include <cubos/engine/render/mesh/pool.hpp>
#include <cubos/engine/render/mesh/task.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::Window;
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
        struct Entry
        {
            RenderMeshPool::BucketId firstBucketId{};
            Asset<VoxelGrid> asset{};
            int referenceCount{};
        };

        ThreadPool pool;
        std::unordered_map<uuids::uuid, Entry> entries;

        State(std::size_t threadCount)
            : pool(threadCount)
        {
        }
    };
} // namespace

/// @brief Generates faces for the given voxel grid.
/// @param grid Voxel grid.
/// @param[out] faces Faces generated from the voxel grid.

void cubos::engine::renderMeshPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);
    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(renderVoxelsPlugin);

    cubos.uninitResource<RenderMeshPool>();
    cubos.uninitResource<State>();

    cubos.component<RenderMesh>();
    cubos.component<RenderMeshTask>();

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
        .call([](Commands cmds, State& state, Assets& assets, Query<Entity, const RenderVoxelGrid&> query) {
            for (auto [ent, grid] : query)
            {
                // If the asset has never been loaded, or it has been updated since the last meshing, update the entry
                // and queue a meshing task. Otherwise, just reuse the existing mesh.
                auto& entry = state.entries[grid.asset.getId()];
                entry.referenceCount += 1;
                if (entry.asset.isNull() || assets.update(entry.asset))
                {
                    entry.asset = grid.asset;
                    assets.update(entry.asset); // Make sure the stored handle is up-to-date.

                    // Spawn a thread task to mesh the voxels
                    RenderMeshTask renderMesh{};
                    state.pool.addTask([&assets, task = renderMesh.task, asset = grid.asset]() mutable {
                        // Load the voxel data.
                        auto grid = assets.read<VoxelGrid>(asset);

                        // Generate a mesh from the voxel data.
                        std::vector<RenderMeshVertex> vertices{};
                        RenderMeshVertex::generate(*grid, vertices);

                        // Output the task result.
                        task.finish(std::move(vertices));
                    });

                    cmds.add(ent, std::move(renderMesh));
                }
                else
                {
                    cmds.add(ent, RenderMesh{.firstBucketId = entry.firstBucketId});
                }
            }
        });

    cubos.system("poll RenderMeshTasks")
        .call([](Commands cmds, State& state, Query<Entity, const RenderVoxelGrid&, RenderMeshTask&> query,
                 RenderMeshPool& pool) {
            for (auto [ent, grid, renderMesh] : query)
            {
                if (renderMesh.task.isDone())
                {
                    auto vertices = renderMesh.task.result();
                    auto firstBucketId = pool.allocate(vertices.data(), vertices.size());
                    cmds.remove<RenderMeshTask>(ent);
                    cmds.remove<LoadRenderVoxels>(ent);
                    cmds.add(ent, RenderMesh{.firstBucketId = firstBucketId});
                    CUBOS_INFO("Finished meshing voxel grid asset {} ({} vertices), allocated new render bucket",
                               grid.asset.getId(), vertices.size());

                    auto& entry = state.entries[grid.asset.getId()];
                    if (entry.firstBucketId != RenderMeshPool::BucketId::Invalid)
                    {
                        pool.deallocate(entry.firstBucketId);
                        CUBOS_INFO("Deallocated old render mesh bucket of asset {}", grid.asset.getId());
                    }
                    entry.firstBucketId = firstBucketId;
                }
            }
        });

    cubos.observer("decrease reference count of removed RenderMeshes")
        .onRemove<RenderMesh>()
        .call([](Query<Entity, const RenderVoxelGrid&> query, RenderMeshPool& pool, State& state) {
            for (auto [ent, grid] : query)
            {
                auto& entry = state.entries[grid.asset.getId()];
                entry.referenceCount -= 1;
                if (entry.referenceCount == 0)
                {
                    pool.deallocate(entry.firstBucketId);
                    state.entries.erase(grid.asset.getId());
                    CUBOS_INFO("Deallocated render mesh bucket of asset {} (no more references)", grid.asset.getId());
                }
            }
        });
}
