#ifndef CUBOS_ENGINE_ECS_DRAW_SYSTEM_HPP
#define CUBOS_ENGINE_ECS_DRAW_SYSTEM_HPP

#include <cubos/core/ecs/query.hpp>

#include <cubos/engine/gl/frame.hpp>
#include <cubos/engine/ecs/grid.hpp>
#include <cubos/engine/ecs/local_to_world.hpp>

namespace cubos::engine::ecs
{
    /// Draws all entities with a Grid and a LocalToWorld component to the frame resource.
    void drawSystem(gl::Frame& frame, core::ecs::Query<const Grid&, const LocalToWorld&> query);
} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_DRAW_SYSTEM_HPP
