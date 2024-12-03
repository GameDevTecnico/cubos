#include <random>

#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/defaults/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("9671dc21-e800-4e8b-ab15-e18d9749d233");

glm::vec2 n22(glm::vec2 p)
{
    glm::vec3 a = glm::fract(glm::vec3(p.x, p.y, p.x) * glm::vec3(123.34, 234.34, 345.65));
    a += glm::dot(a, a + glm::vec3(34.45F));
    return glm::fract(glm::vec2(a.x * a.y, a.y * a.z));
}

glm::vec2 get_gradient(glm::vec2 pos)
{
    float twoPi = 6.283185F;
    float angle = n22(pos).x * twoPi;
    return glm::vec2(cos(angle), sin(angle));
}

float perlin_noise(glm::vec2 uv, float cells_count)
{
    glm::vec2 pos_in_grid = uv * cells_count;
    glm::vec2 cell_pos_in_grid = floor(pos_in_grid);
    glm::vec2 local_pos_in_cell = (pos_in_grid - cell_pos_in_grid);
    glm::vec2 blend = local_pos_in_cell * local_pos_in_cell * (3.0f - 2.0f * local_pos_in_cell);

    glm::vec2 left_top = cell_pos_in_grid + glm::vec2(0, 1);
    glm::vec2 right_top = cell_pos_in_grid + glm::vec2(1, 1);
    glm::vec2 left_bottom = cell_pos_in_grid + glm::vec2(0, 0);
    glm::vec2 right_bottom = cell_pos_in_grid + glm::vec2(1, 0);

    float left_top_dot = glm::dot(pos_in_grid - left_top, get_gradient(left_top));
    float right_top_dot = glm::dot(pos_in_grid - right_top, get_gradient(right_top));
    float left_bottom_dot = glm::dot(pos_in_grid - left_bottom, get_gradient(left_bottom));
    float right_bottom_dot = glm::dot(pos_in_grid - right_bottom, get_gradient(right_bottom));

    float noise_value = glm::mix(glm::mix(left_bottom_dot, right_bottom_dot, blend.x),
                                 glm::mix(left_top_dot, right_top_dot, blend.x), blend.y);

    return (0.5F + 0.5F * (noise_value / 0.7F));
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    /// [Adding the plugins]
    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(voxelsPlugin);
    cubos.plugin(renderDefaultsPlugin);
    /// [Adding the plugins]

    cubos.plugin(scenePlugin);

    cubos.startupSystem("configure settings").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
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
            commands.spawn(assets.read(SceneAsset)->blueprint);
        });

    cubos.system("update chunks")
        .call([](Commands commands, Assets& assets, Query<Entity, const RenderVoxelGrid&, const Position&> chunks,
                 Query<const Camera&, const Position&> cameras) {
            constexpr float chunkSize = 32.0F;
            constexpr int genRadiusChunks = 8;
            for (auto [camera, position] : cameras)
            {
                for (int x = -genRadiusChunks; x < genRadiusChunks; x++)
                {
                    for (int z = -genRadiusChunks; z < genRadiusChunks; z++)
                    {
                        float posX = float(x + static_cast<int>(position.vec.x / chunkSize)) * chunkSize;
                        float posZ = float(z + static_cast<int>(position.vec.z / chunkSize)) * chunkSize;
                        bool hasChunk = false;
                        for (auto [entity, grid, gridPosition] : chunks)
                        {
                            if (gridPosition.vec.x == posX && gridPosition.vec.z == posZ)
                            {
                                hasChunk = true;
                                break;
                            }
                        }
                        if (!hasChunk)
                        {
                            // Create a grid with random materials.
                            glm::uvec3 gridSize = {static_cast<int>(chunkSize), 32, static_cast<int>(chunkSize)};
                            auto voxelGrid = VoxelGrid{gridSize};
                            for (glm::uint gx = 0; gx < gridSize.x; gx++)
                            {
                                for (glm::uint gz = 0; gz < gridSize.z; gz++)
                                {
                                    for (glm::uint gy = 0;
                                         gy < static_cast<glm::uint>(
                                                  static_cast<float>(gridSize.y) *
                                                  perlin_noise({float(gx) + posX, float(gz) + posZ}, 0.05F));
                                         gy++)
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
                float minX = float(-genRadiusChunks + static_cast<int>(position.vec.x / chunkSize)) * chunkSize;
                float maxX = float(genRadiusChunks + static_cast<int>(position.vec.x / chunkSize)) * chunkSize;
                float minZ = float(-genRadiusChunks + static_cast<int>(position.vec.z / chunkSize)) * chunkSize;
                float maxZ = float(genRadiusChunks + static_cast<int>(position.vec.z / chunkSize)) * chunkSize;
                for (auto [entity, grid, gridPosition] : chunks)
                {
                    if (gridPosition.vec.x < minX || gridPosition.vec.x > maxX || gridPosition.vec.z < minZ ||
                        gridPosition.vec.z > maxZ)
                    {
                        removedChunks.push_back(entity);
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
            position.vec += glm::vec3(0.0F, 0.0F, -30.0F * dt.value());
        }
    });
    cubos.run();
}
