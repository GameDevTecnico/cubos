#ifndef CUBOS_CORE_GL_OGL_RENDER_DEVICE_HPP
#define CUBOS_CORE_GL_OGL_RENDER_DEVICE_HPP

#include <cubos/core/gl/render_device.hpp>

namespace cubos::core::gl
{
    /// Render device implementation using OpenGL.
    /// @see RenderDevice.
    class OGLRenderDevice : public RenderDevice
    {
    public:
        OGLRenderDevice();

        virtual Framebuffer createFramebuffer(const FramebufferDesc& desc) override;
        virtual void setFramebuffer(Framebuffer fb) override;
        virtual RasterState createRasterState(const RasterStateDesc& desc) override;
        virtual void setRasterState(RasterState rs) override;
        virtual DepthStencilState createDepthStencilState(const DepthStencilStateDesc& desc) override;
        virtual void setDepthStencilState(DepthStencilState dss) override;
        virtual BlendState createBlendState(const BlendStateDesc& desc) override;
        virtual void setBlendState(BlendState bs) override;
        virtual Sampler createSampler(const SamplerDesc& desc) override;
        virtual Texture1D createTexture1D(const Texture1DDesc& desc) override;
        virtual Texture2D createTexture2D(const Texture2DDesc& desc) override;
        virtual Texture2DArray createTexture2DArray(const Texture2DArrayDesc& desc) override;
        virtual Texture3D createTexture3D(const Texture3DDesc& desc) override;
        virtual CubeMap createCubeMap(const CubeMapDesc& desc) override;
        virtual CubeMapArray createCubeMapArray(const CubeMapArrayDesc& desc) override;
        virtual ConstantBuffer createConstantBuffer(std::size_t size, const void* data, Usage usage) override;
        virtual IndexBuffer createIndexBuffer(std::size_t size, const void* data, IndexFormat format, Usage usage) override;
        virtual void setIndexBuffer(IndexBuffer ib) override;
        virtual VertexBuffer createVertexBuffer(std::size_t size, const void* data, Usage usage) override;
        virtual VertexArray createVertexArray(const VertexArrayDesc& desc) override;
        virtual void setVertexArray(VertexArray va) override;
        virtual ShaderStage createShaderStage(Stage stage, const char* src) override;
        virtual ShaderPipeline createShaderPipeline(ShaderStage vs, ShaderStage ps) override;
        virtual ShaderPipeline createShaderPipeline(ShaderStage vs, ShaderStage gs, ShaderStage ps) override;
        virtual ShaderPipeline createShaderPipeline(ShaderStage cs) override;
        virtual void setShaderPipeline(ShaderPipeline pipeline) override;
        virtual void clearColor(float r, float g, float b, float a) override;
        virtual void clearTargetColor(std::size_t target, float r, float g, float b, float a) override;
        virtual void clearDepth(float depth) override;
        virtual void clearStencil(int stencil) override;
        virtual void drawTriangles(std::size_t offset, std::size_t count) override;
        virtual void drawTrianglesIndexed(std::size_t offset, std::size_t count) override;
        virtual void drawTrianglesInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount) override;
        virtual void drawTrianglesIndexedInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount) override;
        virtual void dispatchCompute(std::size_t x, std::size_t y, std::size_t z) override;
        virtual void memoryBarrier(MemoryBarriers barriers) override;
        virtual void setViewport(int x, int y, int w, int h) override;
        virtual void setScissor(int x, int y, int w, int h) override;
        virtual int getProperty(Property prop) override;

    private:
        int currentIndexFormat;
        std::size_t currentIndexSz;

        RasterState defaultRS;
        DepthStencilState defaultDSS;
        BlendState defaultBS;
    };
} // namespace cubos::core::gl

#endif // CUBOS_CORE_GL_OGL_RENDER_DEVICE_HPP
