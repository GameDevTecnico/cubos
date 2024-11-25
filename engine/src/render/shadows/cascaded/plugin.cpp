#include <algorithm>

#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/shadows/cascaded/plugin.hpp>
#include <cubos/engine/render/shadows/casters/directional_caster.hpp>
#include <cubos/engine/render/shadows/casters/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::createCascadedShadowMapsTag);
CUBOS_DEFINE_TAG(cubos::engine::drawToCascadedShadowMapsTag);

void cubos::engine::cascadedShadowMapsPlugin(Cubos& cubos)
{
    cubos.depends(cameraPlugin);
    cubos.depends(shadowCastersPlugin);
    cubos.depends(windowPlugin);

    cubos.tag(createCascadedShadowMapsTag);
    cubos.tag(drawToCascadedShadowMapsTag).after(createCascadedShadowMapsTag);

    cubos.system("create cascaded shadow maps")
        .tagged(createCascadedShadowMapsTag)
        .call([](const Window& window, Query<DirectionalShadowCaster&> query, Query<Entity, const Camera&> cameras) {
            for (auto [caster] : query)
            {
                // Remove shadow maps for cameras that no longer exist
                std::vector<Entity> removedCameras;
                for (auto& [cameraEntity, shadowMap] : caster.shadowMaps)
                {
                    if (!cameras.at(cameraEntity))
                    {
                        removedCameras.push_back(cameraEntity);
                    }
                }
                for (auto [entity, camera] : cameras)
                {
                    if (!camera.active && caster.shadowMaps.contains(entity))
                    {
                        removedCameras.push_back(entity);
                    }
                }
                for (auto& cameraEntity : removedCameras)
                {
                    caster.shadowMaps.erase(cameraEntity);
                }

                caster.updateShadowMaps(window->renderDevice());

                // Create shadow maps for new cameras
                for (auto [entity, camera] : cameras)
                {
                    if (camera.active && !caster.shadowMaps.contains(entity))
                    {
                        caster.shadowMaps[entity] = std::make_shared<DirectionalShadowCaster::ShadowMap>();
                        caster.shadowMaps[entity]->resize(window->renderDevice(), caster.size,
                                                          static_cast<int>(caster.getCurrentSplitDistances().size()) +
                                                              1);
                    }
                }
            }
        });
}
