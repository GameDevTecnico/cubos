#include <unordered_map>

#include <glm/gtc/noise.hpp>
#include <glm/gtx/hash.hpp>

#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/tools/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("9671dc21-e800-4e8b-ab15-e18d9749d233");

struct ChunkMap
{
    CUBOS_ANONYMOUS_REFLECT(ChunkMap);

    std::unordered_map<glm::vec2, bool> chunkExists;
};

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    /// [Adding the plugins]
    cubos.plugin(defaultsPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.plugin(toolsPlugin);
    /// [Adding the plugins]

    cubos.resource<ChunkMap>();

    cubos.startupSystem("configure settings").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
        settings.setInteger("renderMeshPool.bucketCount", 2048);
    });

    cubos.startupSystem("set the palette and environment and spawn the scene")
        .tagged(assetsTag)
        .call([](Commands commands, Assets& assets, RenderPalette& palette, RenderEnvironment& environment) {
            // Create a simple palette with 3 materials (red, green and blue).
            palette.asset = assets.create(VoxelPalette{{
                {{0, 1, 0, 1}},
            }});
            // Environment
            environment.ambient = {0.1F, 0.1F, 0.1F};
            environment.skyGradient[0] = {0.1F, 0.2F, 0.4F};
            environment.skyGradient[1] = {0.6F, 0.6F, 0.8F};
            // Spawn base scene
            commands.spawn(assets.read(SceneAsset)->blueprint());
        });

    cubos.system("update chunks")
        .call([](Commands commands, Assets& assets, Query<Entity, const RenderVoxelGrid&, const Position&> chunks,
                 Query<const Camera&, const Position&> cameras, ChunkMap& chunkMap) {
            constexpr float ChunkSize = 32.0F;
            constexpr int GenRadiusChunks = 16;
            for (auto [camera, position] : cameras)
            {
                if (!camera.active)
                {
                    continue;
                }
                for (int x = -GenRadiusChunks; x < GenRadiusChunks; x++)
                {
                    for (int z = -GenRadiusChunks; z < GenRadiusChunks; z++)
                    {
                        float posX = float(x + static_cast<int>(position.vec.x / ChunkSize)) * ChunkSize;
                        float posZ = float(z + static_cast<int>(position.vec.z / ChunkSize)) * ChunkSize;
                        if (!chunkMap.chunkExists.contains(glm::vec2(posX, posZ)))
                        {
                            chunkMap.chunkExists[glm::vec2(posX, posZ)] = true;
                            // Create a perlin noise terrain.
                            glm::uvec3 gridSize = {static_cast<int>(ChunkSize), 32, static_cast<int>(ChunkSize)};
                            auto voxelGrid = VoxelGrid{gridSize};
                            for (glm::uint gx = 0; gx < gridSize.x; gx++)
                            {
                                for (glm::uint gz = 0; gz < gridSize.z; gz++)
                                {
                                    auto noise = 0.5F + 0.5F * glm::perlin(0.05F * glm::vec2(float(gx) + posX,
                                                                                             float(gz) + posZ));
                                    for (glm::uint gy = 0;
                                         gy < static_cast<glm::uint>(static_cast<float>(gridSize.y) * noise); gy++)
                                    {
                                        voxelGrid.set({gx, gy, gz}, 1);
                                    }
                                }
                            }
                            auto gridAsset = assets.create(voxelGrid);
                            // Spawn an entity with a renderable grid component and a identity transform.
                            commands.create()
                                .add(RenderVoxelGrid{gridAsset, {-1.0F, 0.0F, -1.0F}})
                                .add(LocalToWorld{})
                                .add(Position{{posX, 0.0F, posZ}});
                        }
                    }
                }
                std::vector<Entity> removedChunks;
                float minX = float(-GenRadiusChunks + static_cast<int>(position.vec.x / ChunkSize)) * ChunkSize;
                float maxX = float(GenRadiusChunks + static_cast<int>(position.vec.x / ChunkSize)) * ChunkSize;
                float minZ = float(-GenRadiusChunks + static_cast<int>(position.vec.z / ChunkSize)) * ChunkSize;
                float maxZ = float(GenRadiusChunks + static_cast<int>(position.vec.z / ChunkSize)) * ChunkSize;
                for (auto [entity, grid, gridPosition] : chunks)
                {
                    if (gridPosition.vec.x < minX || gridPosition.vec.x > maxX || gridPosition.vec.z < minZ ||
                        gridPosition.vec.z > maxZ)
                    {
                        removedChunks.push_back(entity);
                        chunkMap.chunkExists.erase(glm::vec2(gridPosition.vec.x, gridPosition.vec.z));
                    }
                }
                for (auto& chunkEntity : removedChunks)
                {
                    commands.destroy(chunkEntity);
                }
            }
        });

    cubos.system("move camera").call([](Query<const Camera&, Position&> cameras, DeltaTime& dt) {
        for (auto [camera, position] : cameras)
        {
            if (!camera.active)
            {
                continue;
            }
            position.vec += glm::vec3(0.0F, 0.0F, -30.0F * dt.value());
        }
    });
    cubos.run();
}
