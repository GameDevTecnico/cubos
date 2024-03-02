/// @file
/// @brief Function @ref cubos::core::gl::generateScreenQuad.
/// @ingroup core-gl

#pragma once

#include <cubos/core/gl/render_device.hpp>

namespace cubos::core::gl
{
    /// @brief Creates the resources required to draw a quad that fills the screen and returns its
    /// @ref VertexArray.
    ///
    /// The quad provided by this function consists of two one sided triangles, with vertices
    /// containing the 2D position and UV coordinates. This function assumes the shader pipeline
    /// provided takes as input attributes a vec2 `position` and a vec2 `uv`.
    ///
    /// @param renderDevice RenderDevice to be used for generation of the quad.
    /// @param pipeline Pipeline to associate the quad's VertexArray with.
    /// @param[out] va Handle of the quad's VertexArray.
    CUBOS_CORE_API void generateScreenQuad(RenderDevice& renderDevice, const ShaderPipeline& pipeline, VertexArray& va);
} // namespace cubos::core::gl
