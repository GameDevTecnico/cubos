#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/bloom/plugin.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/deferred_shading/plugin.hpp>
#include <cubos/engine/render/depth/plugin.hpp>
#include <cubos/engine/render/g_buffer/plugin.hpp>
#include <cubos/engine/render/g_buffer_rasterizer/plugin.hpp>
#include <cubos/engine/render/hdr/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/render/mesh/plugin.hpp>
#include <cubos/engine/render/picker/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/render/split_screen/plugin.hpp>
#include <cubos/engine/render/ssao/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/tone_mapping/plugin.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::engine;

static const Asset<Scene> SceneAsset = AnyAsset("05db7643-a8b8-49cd-8c16-0992136cfacf");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");

int main()
{
    Cubos cubos{};

    /// [Adding the plugins]
    cubos.plugin(settingsPlugin);
    cubos.plugin(windowPlugin);
    cubos.plugin(transformPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(voxelsPlugin);
    cubos.plugin(shaderPlugin);
    cubos.plugin(renderTargetPlugin);
    cubos.plugin(gBufferPlugin);
    cubos.plugin(renderPickerPlugin);
    cubos.plugin(renderDepthPlugin);
    cubos.plugin(hdrPlugin);
    cubos.plugin(cameraPlugin);
    cubos.plugin(renderVoxelsPlugin);
    cubos.plugin(renderMeshPlugin);
    cubos.plugin(gBufferRasterizerPlugin);
    cubos.plugin(lightsPlugin);
    cubos.plugin(ssaoPlugin);
    cubos.plugin(deferredShadingPlugin);
    cubos.plugin(toneMappingPlugin);
    cubos.plugin(splitScreenPlugin);
    cubos.plugin(bloomPlugin);
    cubos.tag(bloomTag).after(deferredShadingTag);
    /// [Adding the plugins]

    cubos.plugin(scenePlugin);

    cubos.startupSystem("configure settings").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", SAMPLE_ASSETS_FOLDER);
        settings.setBool("window.vSync", false);
    });

    cubos.startupSystem("set the palette and environment and spawn the scene")
        .tagged(assetsTag)
        .call([](Commands commands, const Assets& assets, RenderPalette& palette, RenderEnvironment& environment) {
            palette.asset = PaletteAsset;
            environment.ambient = {0.1F, 0.1F, 0.1F};
            environment.skyGradient[0] = {0.1F, 0.2F, 0.4F};
            environment.skyGradient[1] = {0.6F, 0.6F, 0.8F};
            commands.spawn(assets.read(SceneAsset)->blueprint);
        });

    cubos.system("log FPS").call([](const DeltaTime& dt) { CUBOS_INFO("FPS: {}", 1.0F / dt.unscaledValue); });

    cubos.run();
}
