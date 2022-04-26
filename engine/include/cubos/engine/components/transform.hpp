#ifndef CUBOS_ENGINE_COMPONENTS_TRANSFORM_HPP
#define CUBOS_ENGINE_COMPONENTS_TRANSFORM_HPP

#include <glm/ext.hpp>
#include <cubos/core/ecs/vec_storage.hpp>

namespace cubos::engine::components {

    struct Transform {
        using Storage = core::ecs::VecStorage<Transform>;
        glm::vec3 position{0};
        glm::quat rotation = glm::angleAxis(0.0f, glm::vec3{0, 1, 0});
        glm::vec3 scale{1};
    };

}

#endif // CUBOS_ENGINE_COMPONENTS_TRANSFORM_HPP
