#include <cubos/engine/ecs/draw_system.hpp>
#include <cubos/engine/data/asset_manager.hpp>

#include <glm/gtc/matrix_transform.hpp>

void cubos::engine::ecs::drawSystem(gl::Frame& frame, data::AssetManager& assetMgr,
                                    core::ecs::Query<const Grid&, const LocalToWorld&> query)
{
    for (auto [entity, grid, localToWorld] : query)
    {
        auto asset = assetMgr.load(grid.handle);
        frame.draw(asset->rendererGrid, localToWorld.mat * glm::translate(glm::mat4(1.0f), grid.modelOffset));
    }
}
