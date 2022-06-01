
#ifndef CUBOS_CORE_GL_UTIL_HPP
#define CUBOS_CORE_GL_UTIL_HPP

#include <cubos/core/gl/render_device.hpp>

namespace cubos::core::gl
{
    /// @brief Creates the resources required to draw a quad that fills the screen and returns its VertexArray.
    /// @details The quad provided by this function consists of two one sided triangles, with vertices containing the 2D
    /// position and UV coordinates. This function assumes the shader pipeline provided takes as input attributes a vec2
    /// 'position' and a vec2 'uv'.
    /// @param renderDevice RenderDevice to be used for generation of the quad.
    /// @param pipeline Pipeline to associate the quad's VertexArray with.
    /// @param va Reference in which the quad's VertexArray will be stored.
    void generateScreenQuad(RenderDevice& renderDevice, const ShaderPipeline& pipeline, VertexArray& va);
} // namespace cubos::core::gl

#endif // CUBOS_CORE_GL_UTIL_HPP
