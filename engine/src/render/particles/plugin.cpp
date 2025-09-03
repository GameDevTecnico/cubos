#include <cubos/core/gl/util.hpp>
#include <cubos/core/io/window.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/particles/plugin.hpp>
#include <cubos/engine/render/particles/spawner.hpp>
#include <cubos/engine/render/shader/plugin.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::core::gl;
using cubos::core::io::Window;
CUBOS_DEFINE_TAG(cubos::engine::setupParticleSystemTag);
CUBOS_DEFINE_TAG(cubos::engine::setupBuffersTag);
CUBOS_DEFINE_TAG(cubos::engine::particleSystemTag);
namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        ShaderPipeline pipeline;
        ShaderBindingPoint firstColorBP;
        ShaderBindingPoint secondColorBP;
        ShaderBindingPoint cameraBP;

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline)
            : pipeline(pipeline)
        {
            firstColorBP = pipeline->getBindingPoint("firstColor");
            secondColorBP = pipeline->getBindingPoint("secondColor");
            cameraBP = pipeline->getBindingPoint("Camera");

            CUBOS_ASSERT(firstColorBP && secondColorBP && cameraBP,
                         "firstColor, secondColor and Camera binding points must exist");
            (void)renderDevice;
        }
    };
} // namespace

void cubos::engine::particleSystemPlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("da975621-f78c-4f43-bbfd-10d5b249f3db");
    static const Asset<Shader> PixelShader = AnyAsset("5e121c47-5e6d-4f7e-aa67-f98a62c9732b");

    cubos.depends(windowPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(shaderPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(transformPlugin);

    cubos.startupTag(cubos::engine::setupParticleSystemTag);
    cubos.startupTag(cubos::engine::setupBuffersTag);
    cubos.tag(cubos::engine::particleSystemTag);
    cubos.uninitResource<State>();

    cubos.component<ParticleSpawner>();

    cubos.startupSystem("setup Particle System")
        .tagged(setupParticleSystemTag)
        .tagged(assetsTag)
        .after(windowInitTag)
        .call([](Commands cmds, const Window& window, Assets& assets) {
            auto& rd = window->renderDevice();

            // Load shaders and create pipeline
            auto vs = assets.read(VertexShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();

            auto pipeline = rd.createShaderPipeline(vs, ps);
            if (pipeline == nullptr)
            {
                CUBOS_ERROR("Failed to create particle system shader pipeline");
                return;
            }

            cmds.emplaceResource<State>(rd, pipeline);
            CUBOS_INFO("Particle system initialized");
        });

    cubos.system("render particle systems")
        .tagged(particleSystemTag)
        .call([](const Window& window, const State& state,
                 Query<const LocalToWorld&, const Camera&, const DrawsTo&> cameras,
                 Query<ParticleSpawner&, const LocalToWorld&> spawners) {
            auto& rd = window->renderDevice();

            for (auto [cameraLtw, camera, drawsTo] : cameras)
            {
                if (!camera.active)
                {
                    continue;
                }
                // Calculate view matrix (inverse of camera's local-to-world transform)
                glm::mat4 viewMatrix = cameraLtw.inverse();
                for (auto [spawner, spawnerLtw] : spawners)
                {
                    if (spawner.particleVAO == nullptr)
                    {
                        spawner.setupBuffers(rd, state.pipeline);
                        if (spawner.particleVAO == nullptr)
                        {
                            CUBOS_ERROR("Failed to setup particle VAO for spawner!");
                            continue;
                        }
                    }

                    CUBOS_WARN("Particles array with:{} particles", spawner.particles.size());
                    spawner.draw(rd, state.pipeline, viewMatrix, camera.projection);
                }
            }
        });
}