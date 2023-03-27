#pragma once

#include <cubos/engine/cubos.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace cubos::engine
{
    /// Plugin which adds scenes to CUBOS. Scenes are the asset equivalent to ECS blueprints.
    ///
    /// @details Registers the `SceneBridge` asset bridge with the `.scn` extension. Loads assets
    /// of the `Scene` type.
    ///
    /// @param cubos CUBOS. main class.
    void scenePlugin(Cubos& cubos);
} // namespace cubos::engine
