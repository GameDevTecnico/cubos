#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/memory/stream.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/renderer/light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/renderer.hpp>
#include <cubos/engine/transform/plugin.hpp>

using cubos::core::Settings;
using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::World;
using cubos::core::ecs::Write;
using namespace cubos::engine;

static void settings(Write<Settings> settings)
{
    settings->setBool("assets.io.enabled", false);
}

static void setupScene(Commands commands, Write<Assets> assets, Write<ActiveCamera> camera, Write<Renderer> renderer)
{
    using cubos::core::gl::Grid;
    using cubos::core::gl::Palette;

    // Create a simple palette with 3 materials (red, green and blue).
    (*renderer)->setPalette(Palette{{
        {{1, 0, 0, 1}},
        {{0, 1, 0, 1}},
        {{0, 0, 1, 1}},
    }});

    // Create a 2x2x2 grid whose voxels alternate between the materials defined above.
    auto gridAsset = assets->create(Grid{{2, 2, 2}, {1, 2, 3, 1, 2, 3, 1, 2}});

    // Spawn an entity with a renderable grid component and a identity transform.
    commands.create(RenderableGrid{gridAsset}, LocalToWorld{});

    // Spawn the camera entity.
    camera->entity = commands.create()
                         .add(Camera{.fovY = 60.0F, .zNear = 0.1F, .zFar = 100.0F})
                         .add(LocalToWorld{})
                         .add(Position{{5.0F, 5.0F, -10.0F}})
                         .add(Rotation{glm::quatLookAt(glm::vec3{0.0F, 0.0F, 1.0F}, glm::vec3{0.0F, 1.0F, 0.0F})})
                         .entity();

    // Spawn a point light.
    commands.create()
        .add(PointLight{.color = {1.0F, 1.0F, 1.0F}, .intensity = 1.0F, .range = 10.0F})
        .add(LocalToWorld{})
        .add(Position{{1.0F, 3.0F, -2.0F}});
}

int main(int argc, char** argv)
{
    Cubos cubos(argc, argv);

    cubos.addPlugin(rendererPlugin);

    cubos.startupSystem(settings).tagged("cubos.settings");
    cubos.startupSystem(setupScene).afterTag("cubos.renderer.init");
    cubos.run();
}
