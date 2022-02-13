
#ifndef CUBOS_GL_UTIL_HPP
#define CUBOS_GL_UTIL_HPP

#include <cubos/gl/render_device.hpp>

#include <map>
#include <functional>

namespace cubos::gl
{
    class Util
    {
    private:
        struct QuadBuffers
        {
            QuadBuffers(VertexBuffer vb, IndexBuffer ib);
            VertexBuffer vb;
            IndexBuffer ib;
        };
        static std::map<RenderDevice*, QuadBuffers> bufferMap;

    public:
        /// @brief Gets the resources required to draw a quad that fills the screen.
        /// @details The quad provided by this function consists of two one sided triangles triangles. To draw the quad,
        /// one should first set the provided VertexArray and IndexBuffer using RenderDevice::setVertexArray and
        /// RenderDevice::setIndexBuffer respectively. The quad can then be rendered by calling
        /// RenderDevice::drawTrianglesIndexed with @p offset 0 and @p count 6.
        /// @param renderDevice RenderDevice to be used for generation of the quad.
        /// @param pipeline Pipeline to associate the quad's VertexArray with.
        /// @param va Reference in which the quad's VertexArray will be stored.
        /// @param ib Reference in which the quad's IndexBuffer will be stored.
        static void getScreenQuad(RenderDevice& renderDevice, ShaderPipeline pipeline, VertexArray& va,
                                  IndexBuffer& ib);
    };
} // namespace cubos::gl

#endif // CUBOS_GL_UTIL_HPP
