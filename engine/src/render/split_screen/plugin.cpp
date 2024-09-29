#include <glm/vec2.hpp>

#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/split_screen/plugin.hpp>
#include <cubos/engine/render/split_screen/split_screen.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_DEFINE_TAG(cubos::engine::splitScreenTag);

/// @brief Splits the viewport recursively for the given cameras.
/// @param position Viewport position.
/// @param size Viewport size.
/// @param count How many cameras need to be fitted in to the given viewport.
/// @param activeCameras Output array where the viewports will be set.
static void setViewportCameras(glm::ivec2 position, glm::ivec2 size, int count,
                               std::vector<glm::ivec2>::iterator positions, std::vector<glm::ivec2>::iterator sizes)
{
    if (count == 1)
    {
        positions[0] = position;
        sizes[0] = size;
    }
    else if (count >= 2)
    {
        glm::ivec2 splitSize;
        glm::ivec2 splitOffset;

        // Split along the largest axis.
        if (size.x > size.y)
        {
            splitSize = {(size.x + 1) / 2, size.y};
            splitOffset = {size.x / 2, 0};
        }
        else
        {
            splitSize = {size.x, (size.y + 1) / 2};
            splitOffset = {0, size.y / 2};
        }

        setViewportCameras(position, splitSize, count / 2, positions, sizes);
        setViewportCameras(position + splitOffset, splitSize, (count + 1) / 2, positions + (count / 2),
                           sizes + (count / 2));
    }
}

void cubos::engine::splitScreenPlugin(Cubos& cubos)
{
    cubos.depends(renderTargetPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(transformPlugin);

    cubos.component<SplitScreen>();

    cubos.tag(splitScreenTag).after(resizeRenderTargetTag);

    cubos.system("split screen for each DrawsTo relation")
        .tagged(splitScreenTag)
        .call([](Query<Entity, const RenderTarget&> targets,
                 Query<const LocalToWorld&, const Camera&, DrawsTo&, const SplitScreen&> cameras) {
            for (auto [targetEnt, target] : targets)
            {
                int cameraCount = 0;

                // Find the cameras that draw to the target.
                for (auto [localToWorld, camera, drawsTo, splitScreen] : cameras.pin(1, targetEnt))
                {
                    // Ignore unused argument warnings
                    (void)splitScreen;

                    if (!camera.active)
                    {
                        continue;
                    }

                    cameraCount += 1;
                }

                std::vector<glm::ivec2> positions(static_cast<unsigned long>(cameraCount));
                std::vector<glm::ivec2> sizes(static_cast<unsigned long>(cameraCount));

                setViewportCameras({0, 0}, target.size, cameraCount, positions.begin(), sizes.begin());

                unsigned long i = 0;
                for (auto [localToWorld, camera, drawsTo, splitScreen] : cameras.pin(1, targetEnt))
                {
                    // Ignore unused argument warnings
                    (void)splitScreen;

                    if (!camera.active)
                    {
                        continue;
                    }

                    drawsTo.viewportOffset = static_cast<glm::vec2>(positions[i]) / static_cast<glm::vec2>(target.size);
                    drawsTo.viewportSize = static_cast<glm::vec2>(sizes[i]) / static_cast<glm::vec2>(target.size);
                    i++;
                }
            }
        });
}
