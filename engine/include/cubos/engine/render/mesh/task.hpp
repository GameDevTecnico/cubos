/// @file
/// @brief Component @ref cubos::engine::RenderMeshTask.
/// @ingroup render-mesh-plugin

#pragma once

#include <vector>

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/thread/task.hpp>

#include <cubos/engine/render/mesh/vertex.hpp>

namespace cubos::engine
{
    /// @brief Component used to track meshing tasks for @ref RenderVoxelGrid components.
    /// @ingroup render-mesh-plugin
    struct CUBOS_ENGINE_API RenderMeshTask
    {
        CUBOS_REFLECT;

        /// @brief Meshing task. When the task finishes, the component is removed and a @ref RenderMesh component is
        /// created.
        core::thread::Task<std::vector<RenderMeshVertex>> task;
    };
} // namespace cubos::engine
