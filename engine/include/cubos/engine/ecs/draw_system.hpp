#ifndef CUBOS_ENGINE_ECS_DRAW_SYSTEM_HPP
#define CUBOS_ENGINE_ECS_DRAW_SYSTEM_HPP

#include <cubos/core/ecs/world.hpp>
#include <cubos/core/ecs/iterating_system.hpp>

#include <cubos/engine/gl/frame.hpp>
#include <cubos/engine/ecs/grid.hpp>
#include <cubos/engine/ecs/local_to_world.hpp>

namespace cubos::engine::ecs
{
    /// Draws all entities with a Grid and a LocalToWorld component to a frame.
    class DrawSystem : public core::ecs::IteratingSystem<Grid, LocalToWorld>
    {
    public:
        /// @param frame The frame to draw to.
        DrawSystem(gl::Frame& frame);

    private:
        virtual void process(core::ecs::World& World, uint64_t entity, Grid& grid, LocalToWorld& localToWorld) override;

        gl::Frame& frame; ///< The frame to draw to.
    };
} // namespace cubos::engine::ecs

#endif // CUBOS_ENGINE_ECS_DRAW_SYSTEM_HPP
