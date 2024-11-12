#include <cubos/core/io/window.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/render/active/plugin.hpp>
#include <cubos/engine/render/shadow_atlas/plugin.hpp>
#include <cubos/engine/render/shadow_atlas/shadow_atlas.hpp>
#include <cubos/engine/render/shadows/plugin.hpp>
#include <cubos/engine/render/shadows/spot_caster.hpp>
#include <cubos/engine/window/plugin.hpp>

using cubos::core::io::Window;

CUBOS_DEFINE_TAG(cubos::engine::createShadowAtlasTag);
CUBOS_DEFINE_TAG(cubos::engine::reserveShadowCastersTag);
CUBOS_DEFINE_TAG(cubos::engine::drawToShadowAtlasTag);

void cubos::engine::shadowAtlasPlugin(Cubos& cubos)
{
    cubos.depends(shadowsPlugin);
    cubos.depends(windowPlugin);

    cubos.resource<ShadowAtlas>();

    cubos.tag(createShadowAtlasTag);
    cubos.tag(reserveShadowCastersTag).after(createShadowAtlasTag);
    cubos.tag(drawToShadowAtlasTag).after(reserveShadowCastersTag);

    cubos.system("create ShadowAtlas").tagged(createShadowAtlasTag).call([](const Window& window, ShadowAtlas& atlas) {
        if (atlas.getSize() != atlas.configSize)
        {
            atlas.resize(window->renderDevice());

            CUBOS_INFO("Resized ShadowAtlas to {}x{}", atlas.getSize().x, atlas.getSize().y);
        }

        // New frame, hint that the shadow atlas texture needs to be cleared.
        atlas.cleared = false;
    });

    cubos.system("reserve space for shadow casters")
        .tagged(reserveShadowCastersTag)
        .call([](ShadowAtlas& atlas, Query<SpotShadowCaster&, const Active&> casters) {
            atlas.slots.clear();
            atlas.slotsMap.clear();
            atlas.slots.push_back(
                std::make_shared<ShadowAtlas::Slot>(glm::vec2(1.0F, 1.0F), glm::vec2(0.0F, 0.0F), -1));

            int id = 1;

            for (auto [caster, active] : casters)
            {
                if (!active.active)
                {
                    continue;
                }

                bool foundSlot = false;
                caster.baseSettings.id = id++;
                for (const auto& slot : atlas.slots)
                {
                    if (slot->casterId == -1)
                    {
                        slot->casterId = caster.baseSettings.id;
                        atlas.slotsMap[caster.baseSettings.id] = slot;
                        foundSlot = true;
                        break;
                    }
                }
                if (!foundSlot)
                {
                    // Subdivide largest slot, which is always the first
                    auto oldSlot = atlas.slots.at(0);
                    auto newSize = oldSlot->size / glm::vec2(2.0F);

                    atlas.slots.erase(atlas.slots.begin());
                    oldSlot->size = newSize;
                    atlas.slots.push_back(oldSlot);

                    for (int i = 1; i < 4; i++)
                    {
                        // Intentional truncation of (i / 2) so that the result is always 0.0 or 1.0
                        // NOLINTBEGIN(bugprone-integer-division)
                        auto newOffset = oldSlot->offset + glm::vec2(newSize.x * static_cast<float>(i % 2),
                                                                     newSize.y * static_cast<float>(i / 2));
                        // NOLINTEND(bugprone-integer-division)
                        auto newSlot = std::make_shared<ShadowAtlas::Slot>(newSize, newOffset, -1);
                        atlas.slots.push_back(newSlot);
                    }

                    // Re-attempt
                    for (const auto& slot : atlas.slots)
                    {
                        if (slot->casterId == -1)
                        {
                            slot->casterId = caster.baseSettings.id;
                            atlas.slotsMap[caster.baseSettings.id] = slot;
                            break;
                        }
                    }
                }
            }
        });
}
