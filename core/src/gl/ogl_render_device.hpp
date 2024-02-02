#pragma once

#include <cubos/core/gl/render_device.hpp>

namespace cubos::core::gl
{
    /// Render device implementation using OpenGL.
    /// @see RenderDevice.
    class OGLRenderDevice : public RenderDevice
    {
    public:
        OGLRenderDevice();

        Framebuffer createFramebuffer(const FramebufferDesc& desc) override;
        void setFramebuffer(Framebuffer fb) override;
        RasterState createRasterState(const RasterStateDesc& desc) override;
        void setRasterState(RasterState rs) override;
        DepthStencilState createDepthStencilState(const DepthStencilStateDesc& desc) override;
        void setDepthStencilState(DepthStencilState dss) override;
        BlendState createBlendState(const BlendStateDesc& desc) override;
        void setBlendState(BlendState bs) override;
        Sampler createSampler(const SamplerDesc& desc) override;
        Texture1D createTexture1D(const Texture1DDesc& desc) override;
        Texture2D createTexture2D(const Texture2DDesc& desc) override;
        Texture2DArray createTexture2DArray(const Texture2DArrayDesc& desc) override;
        Texture3D createTexture3D(const Texture3DDesc& desc) override;
        CubeMap createCubeMap(const CubeMapDesc& desc) override;
        CubeMapArray createCubeMapArray(const CubeMapArrayDesc& desc) override;
        ConstantBuffer createConstantBuffer(std::size_t size, const void* data, Usage usage) override;
        IndexBuffer createIndexBuffer(std::size_t size, const void* data, IndexFormat format, Usage usage) override;
        void setIndexBuffer(IndexBuffer ib) override;
        VertexBuffer createVertexBuffer(std::size_t size, const void* data, Usage usage) override;
        VertexArray createVertexArray(const VertexArrayDesc& desc) override;
        void setVertexArray(VertexArray va) override;
        ShaderStage createShaderStage(Stage stage, const char* src) override;
        ShaderPipeline createShaderPipeline(ShaderStage vs, ShaderStage ps) override;
        ShaderPipeline createShaderPipeline(ShaderStage vs, ShaderStage gs, ShaderStage ps) override;
        ShaderPipeline createShaderPipeline(ShaderStage cs) override;
        void setShaderPipeline(ShaderPipeline pipeline) override;
        void clearColor(float r, float g, float b, float a) override;
        void clearTargetColor(std::size_t target, float r, float g, float b, float a) override;
        void clearDepth(float depth) override;
        void clearStencil(int stencil) override;
        void drawLines(std::size_t offset, std::size_t count) override;
        void drawTriangles(std::size_t offset, std::size_t count) override;
        void drawTrianglesIndexed(std::size_t offset, std::size_t count) override;
        void drawTrianglesInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount) override;
        void drawTrianglesIndexedInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount) override;
        void dispatchCompute(std::size_t x, std::size_t y, std::size_t z) override;
        void memoryBarrier(MemoryBarriers barriers) override;
        void setViewport(int x, int y, int w, int h) override;
        void setScissor(int x, int y, int w, int h) override;
        int getProperty(Property prop) override;

    private:
        int mCurrentIndexFormat;
        std::size_t mCurrentIndexSz;

        RasterState mDefaultRS;
        DepthStencilState mDefaultDSS;
        BlendState mDefaultBS;
    };
} // namespace cubos::core::gl
