/// @file
/// @brief Component @ref cubos::engine::LoadRenderVoxels.
/// @ingroup render-voxels-plugin

#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/api.hpp>

namespace cubos::engine
{
    /// @brief Component used to indicate that a voxel grid should be sent to the GPU.
    ///
    /// To be used in conjunction with a @ref RenderVoxelGrid component. Read its documentation for more information.
    /// Automatically removed after the task is completed.
    ///
    /// @ingroup render-voxels-plugin
    struct CUBOS_ENGINE_API LoadRenderVoxels
    {
        CUBOS_REFLECT;
    };
} // namespace cubos::engine
