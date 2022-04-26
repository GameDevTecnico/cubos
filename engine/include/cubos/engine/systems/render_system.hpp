#ifndef CUBOS_ENGINE_SYSTEM_RENDER_SYSTEM_HPP
#define CUBOS_ENGINE_SYSTEM_RENDER_SYSTEM_HPP

#include <cubos/engine/gl/renderer.hpp>
#include <cubos/core/ecs/world.hpp>

namespace cubos::engine::systems {
    void renderSystem(core::ecs::World &world, gl::Renderer &renderer);
}

#endif // CUBOS_ENGINE_SYSTEM_RENDER_SYSTEM_HPP
