#ifndef CUBOS_ENGINE_PLUGINS_TRANSFORM_HPP
#define CUBOS_ENGINE_PLUGINS_TRANSFORM_HPP

#include <cubos/engine/ecs/transform_system.hpp>

#include <cubos/engine/cubos.hpp>

using namespace cubos::engine;

namespace cubos::engine::plugins
{
    /// Plugin to create and use transforms.
    ///
    /// Stages:
    /// - setMatrix: Sets the glm matrix for the transform.
    /// @param cubos CUBOS. main class
    void transformPlugin(Cubos& cubos);
}; // namespace cubos::engine::plugins

#endif // CUBOS_ENGINE_PLUGINS_TRANSFORM_HPP