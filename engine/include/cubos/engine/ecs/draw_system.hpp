#ifndef CUBOS_ENGINE_ECS_DRAW_SYSTEM_HPP
#define CUBOS_ENGINE_ECS_DRAW_SYSTEM_HPP

#include <cubos/core/ecs/query.hpp>

#include <cubos/engine/gl/frame.hpp>

#include <components/cubos/local_to_world.hpp>
#include <components/cubos/grid.hpp>

namespace cubos::engine::ecs
{
    /// Draws all entities with a Grid and a LocalToWorld component to the frame resource.
    void drawSystem(gl::Frame& frame, data::AssetManager& assetMgr, core::ecs::Query<Grid&, const LocalToWorld&> query);
} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_DRAW_SYSTEM_HPP
