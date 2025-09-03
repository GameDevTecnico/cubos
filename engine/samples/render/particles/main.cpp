#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/defaults/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/particles/plugin.hpp>
#include <cubos/engine/render/particles/spawner.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>
using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("950f0d55-2fdf-4e70-a283-79a2bda10f99");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");

struct Spin
{
    CUBOS_REFLECT;
};

CUBOS_REFLECT_IMPL(Spin)
{
    return cubos::core::ecs::TypeBuilder<Spin>("Spin").build();
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.component<Spin>();

    /// [Adding the plugins]
    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(voxelsPlugin);
    cubos.plugin(renderDefaultsPlugin);
    /// [Adding the plugins]
    cubos.plugin(particleSystemPlugin);
    cubos.plugin(scenePlugin);

    cubos.startupSystem("configure settings").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("set the palette and environment and spawn the scene")
        .tagged(assetsTag)
        .call([](Commands commands, const Assets& assets, RenderPalette& palette, RenderEnvironment& environment) {
            palette.asset = PaletteAsset;
            environment.ambient = {0.01F, 0.01F, 0.01F};
            environment.skyGradient[0] = {0.1F, 0.2F, 0.4F};
            environment.skyGradient[1] = {0.6F, 0.6F, 0.8F};
            commands.spawn(assets.read(SceneAsset)->blueprint());
        });

    cubos.startupSystem("create a voxel grid").tagged(assetsTag).call([](Commands commands, Assets& assets) {
        // Create a 2x2x2 grid whose voxels alternate between the materials defined in the palette.
        auto gridAsset = assets.create(VoxelGrid{{2, 2, 2}, {1, 2, 3, 1, 2, 3, 1, 2}});

        // Spawn entities with the grid.
        // Cube
        commands.create()
            .add(RenderVoxelGrid{gridAsset, {1.0F, 1.0F, 1.0F}})
            .add(LocalToWorld{})
            .add(Position{{3.0F, -3.0F, -17.0F}})
            .add(Scale{1.5F});
        // Floor
        commands.create()
            .add(RenderVoxelGrid{gridAsset, {1.0F, 1.0F, 1.0F}})
            .add(LocalToWorld{})
            .add(Position{{-20.0F, -43.0F, -50.0F}})
            .add(Scale{20.0F});
    });

    cubos.system("spin car").call([](Query<const Spin&, Rotation&> query, DeltaTime& dt) {
        for (auto [spin, rotation] : query)
        {
            rotation.quat = glm::cross(rotation.quat, glm::quat(glm::vec3{0.0F, dt.value(), 0.0F}));
        }
    });

    cubos.startupSystem("spawn particle spawner").after(assetsTag).call([](Commands cmds) {
        glm::vec3 carPos = {3.0f, -3.0f, -17.0f};
        glm::vec3 spawnerPos = carPos + glm::vec3{0.0f, 2.0f, 0.0f};

        ParticleSpawner spawner;
        spawner.maxParticles = 500;
        spawner.emissionRate = 50.0f;
        spawner.minLifetime = 1.0f;
        spawner.maxLifetime = 2.0f;
        spawner.emissionPoint = spawnerPos;
        spawner.emissionRadius = 0.2f;
        spawner.startColor = {1.0f, 0.5f, 0.0f}; // Laranja
        spawner.endColor = {1.0f, 1.0f, 0.0f};   // Amarelo
        spawner.velocity = {0.0f, 2.0f, 0.0f};

        cmds.create().add(spawner).add(LocalToWorld{glm::mat4(1.0f)});
    });
    cubos.run();
}
