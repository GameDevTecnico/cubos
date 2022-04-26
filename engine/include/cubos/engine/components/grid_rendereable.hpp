#ifndef CUBOS_ENGINE_COMPONENTS_GRIDRENDERABLE_HPP
#define CUBOS_ENGINE_COMPONENTS_GRIDRENDERABLE_HPP

#include <cubos/core/ecs/vec_storage.hpp>
#include <cubos/engine/gl/renderer.hpp>

namespace cubos::engine::components {

    struct GridRenderable {
        using Storage = core::ecs::VecStorage<GridRenderable>;
        gl::Renderer::ModelID modelID;
    };

}

#endif // CUBOS_ENGINE_COMPONENTS_GRIDRENDERABLE_HPP