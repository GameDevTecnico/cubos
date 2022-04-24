
#ifndef CUBOS_CORE_GL_UTIL_HPP
#define CUBOS_CORE_GL_UTIL_HPP

#include <cubos/core/gl/render_device.hpp>

namespace cubos::core::gl
{
    /// @brief Creates the resources required to draw a quad that fills the screen and returns its VertexArray and
    /// IndexBuffer.
    /// @details The quad provided by this function consists of two one sided triangles triangles. To draw the quad,
    /// one should first set the provided VertexArray and IndexBuffer using RenderDevice::setVertexArray and
    /// RenderDevice::setIndexBuffer respectively. The quad can then be rendered by calling
    /// RenderDevice::drawTrianglesIndexed with @p offset 0 and @p count 6.
    /// @param renderDevice RenderDevice to be used for generation of the quad.
    /// @param pipeline Pipeline to associate the quad's VertexArray with.
    /// @param va Reference in which the quad's VertexArray will be stored.
    /// @param ib Reference in which the quad's IndexBuffer will be stored.
    void generateScreenQuad(RenderDevice& renderDevice, const ShaderPipeline& pipeline, VertexArray& va,
                            IndexBuffer& ib);
} // namespace cubos::core::gl

#endif // CUBOS_CORE_GL_UTIL_HPP
