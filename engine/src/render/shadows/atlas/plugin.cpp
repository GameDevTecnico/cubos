#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/shadows/atlas/plugin.hpp>
#include <cubos/engine/render/shadows/atlas/point_atlas.hpp>
#include <cubos/engine/render/shadows/atlas/slot.hpp>
#include <cubos/engine/render/shadows/atlas/spot_atlas.hpp>
#include <cubos/engine/render/shadows/casters/caster.hpp>
#include <cubos/engine/render/shadows/casters/plugin.hpp>
#include <cubos/engine/render/shadows/casters/point_caster.hpp>
#include <cubos/engine/render/shadows/casters/spot_caster.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::Window;

using cubos::engine::PointShadowAtlas;
using cubos::engine::ShadowCaster;
using cubos::engine::ShadowMapSlot;
using cubos::engine::SpotShadowAtlas;

CUBOS_DEFINE_TAG(cubos::engine::createShadowAtlasTag);
CUBOS_DEFINE_TAG(cubos::engine::reserveShadowCastersTag);
CUBOS_DEFINE_TAG(cubos::engine::drawToShadowAtlasTag);

static void reserveCasterSlot(std::vector<std::shared_ptr<ShadowMapSlot>>& slots,
                              std::map<int, std::shared_ptr<ShadowMapSlot>>& slotsMap, ShadowCaster& casterBaseSettings,
                              int& id)
{
    bool foundSlot = false;
    casterBaseSettings.id = id++;
    for (const auto& slot : slots)
    {
        if (slot->casterId == -1)
        {
            slot->casterId = casterBaseSettings.id;
            slotsMap[casterBaseSettings.id] = slot;
            foundSlot = true;
            break;
        }
    }
    if (!foundSlot)
    {
        // Subdivide largest slot, which is always the first
        auto oldSlot = slots.at(0);
        auto newSize = oldSlot->size / glm::vec2(2.0F);

        slots.erase(slots.begin());
        oldSlot->size = newSize;
        slots.push_back(oldSlot);

        for (int i = 1; i < 4; i++)
        {
            // Intentional truncation of (i / 2) so that the result is always 0.0 or 1.0
            // NOLINTBEGIN(bugprone-integer-division)
            auto newOffset = oldSlot->offset +
                             glm::vec2(newSize.x * static_cast<float>(i % 2), newSize.y * static_cast<float>(i / 2));
            // NOLINTEND(bugprone-integer-division)
            auto newSlot = std::make_shared<ShadowMapSlot>(newSize, newOffset, -1);
            slots.push_back(newSlot);
        }

        // Re-attempt
        for (const auto& slot : slots)
        {
            if (slot->casterId == -1)
            {
                slot->casterId = casterBaseSettings.id;
                slotsMap[casterBaseSettings.id] = slot;
                break;
            }
        }
    }
}

void cubos::engine::shadowAtlasPlugin(Cubos& cubos)
{
    cubos.depends(shadowCastersPlugin);
    cubos.depends(windowPlugin);

    cubos.resource<SpotShadowAtlas>();
    cubos.resource<PointShadowAtlas>();

    cubos.tag(createShadowAtlasTag);
    cubos.tag(reserveShadowCastersTag).after(createShadowAtlasTag);
    cubos.tag(drawToShadowAtlasTag).after(reserveShadowCastersTag);

    cubos.system("create shadow atlases")
        .tagged(createShadowAtlasTag)
        .call([](const Window& window, SpotShadowAtlas& spotAtlas, PointShadowAtlas& pointAtlas) {
            if (spotAtlas.getSize() != spotAtlas.configSize)
            {
                spotAtlas.resize(window->renderDevice());

                CUBOS_INFO("Resized SpotShadowAtlas to {}x{}", spotAtlas.getSize().x, spotAtlas.getSize().y);
            }
            if (pointAtlas.getSize() != pointAtlas.configSize)
            {
                pointAtlas.resize(window->renderDevice());

                CUBOS_INFO("Resized PointShadowAtlas to {}x{}", pointAtlas.getSize().x, pointAtlas.getSize().y);
            }

            // New frame, hint that the shadow atlas texture needs to be cleared.
            spotAtlas.cleared = false;
            pointAtlas.cleared = false;
        });

    cubos.system("reserve space for shadow casters")
        .tagged(reserveShadowCastersTag)
        .call([](SpotShadowAtlas& spotAtlas, PointShadowAtlas& pointAtlas, Query<SpotShadowCaster&> spotCasters,
                 Query<PointShadowCaster&> pointCasters) {
            spotAtlas.slots.clear();
            pointAtlas.slots.clear();
            spotAtlas.slotsMap.clear();
            pointAtlas.slotsMap.clear();
            spotAtlas.slots.push_back(
                std::make_shared<ShadowMapSlot>(glm::vec2(1.0F, 1.0F), glm::vec2(0.0F, 0.0F), -1));
            pointAtlas.slots.push_back(
                std::make_shared<ShadowMapSlot>(glm::vec2(1.0F, 1.0F), glm::vec2(0.0F, 0.0F), -1));

            int id = 1;

            for (auto [caster] : spotCasters)
            {
                reserveCasterSlot(spotAtlas.slots, spotAtlas.slotsMap, caster.baseSettings, id);
            }

            for (auto [caster] : pointCasters)
            {
                reserveCasterSlot(pointAtlas.slots, pointAtlas.slotsMap, caster.baseSettings, id);
            }
        });
}
