#include <glm/vec2.hpp>

#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/renderer.hpp>
#include <cubos/engine/renderer/viewport.hpp>
#include <cubos/engine/splitscreen/plugin.hpp>
#include <cubos/engine/transform/local_to_world.hpp>

using namespace cubos::engine;

/// @brief Splits the viewport recursively for the given cameras.
/// @param position Viewport position.
/// @param size Viewport size.
/// @param count How many cameras need to be fitted in to the given viewport.
/// @param activeCameras Output array where the viewports will be set.
static void setViewportCameras(glm::ivec2 position, glm::ivec2 size, int count, glm::ivec2* positions,
                               glm::ivec2* sizes)
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
            splitSize = {size.x / 2, size.y};
            splitOffset = {size.x / 2, 0};
        }
        else
        {
            splitSize = {size.x, size.y / 2};
            splitOffset = {0, size.y / 2};
        }

        setViewportCameras(position, splitSize, count / 2, positions, sizes);
        setViewportCameras(position + splitOffset, splitSize, (count + 1) / 2, &positions[count / 2],
                           &sizes[count / 2]);
    }
}

void cubos::engine::splitscreenPlugin(Cubos& cubos)
{
    cubos.system("split screen for each Viewport")
        .tagged("cubos.renderer.frame")
        .call([](Commands commands, Renderer& renderer, const ActiveCameras& activeCameras,
                 Query<const LocalToWorld&, Camera&, Opt<Viewport&>> query) {
            glm::ivec2 positions[4];
            glm::ivec2 sizes[4];
            int cameraCount = 0;

            for (int i = 0; i < 4; ++i) // NOLINT(modernize-loop-convert)
            {
                if (activeCameras.entities[i].isNull())
                {
                    continue;
                }

                if (query.at(activeCameras.entities[i]).contains())
                {
                    cameraCount += 1;
                }
            }

            setViewportCameras({0, 0}, renderer->size(), cameraCount, positions, sizes);

            for (int i = 0; i < 4; ++i) // NOLINT(modernize-loop-convert)
            {
                if (activeCameras.entities[i].isNull())
                {
                    continue;
                }

                if (auto components = query.at(activeCameras.entities[i]))
                {
                    auto& [localToWorld, camera, viewport] = *components;
                    if (!viewport)
                    {
                        commands.add(activeCameras.entities[i], Viewport{.position = positions[i], .size = sizes[i]});
                    }
                    else
                    {
                        viewport->position = positions[i];
                        viewport->size = sizes[i];
                    }
                }
            }
        });
}
