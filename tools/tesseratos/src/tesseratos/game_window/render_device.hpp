/// @file
/// @brief Class @ref tesseratos::GameRenderDevice.
/// @ingroup tesseratos-game-window-plugin

#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/gl/render_device.hpp>
#include <cubos/core/io/window.hpp>

namespace tesseratos
{
    /// @brief Render device implementation which calls another render device, but with a different default framebuffer.
    /// @ingroup tesseratos-game-window-plugin
    class GameRenderDevice : public cubos::core::gl::RenderDevice
    {
    public:
        /// @brief Constructor.
        /// @param renderDevice Real render device.
        GameRenderDevice(RenderDevice& renderDevice);

        /// @brief Recreates the default framebuffers and textures.
        /// @param size New window size.
        void createSwapChain(const glm::uvec2& size);

        /// @brief Swaps the front and back framebuffers.
        void swapBuffers();

        /// @brief Gets the texture with the rendered output.
        /// @return Texture with the rendered output.
        cubos::core::gl::Texture2D output() const;

        // Interface implementation.

        cubos::core::gl::Framebuffer createFramebuffer(const cubos::core::gl::FramebufferDesc& desc) override;
        void setFramebuffer(cubos::core::gl::Framebuffer fb) override;
        cubos::core::gl::RasterState createRasterState(const cubos::core::gl::RasterStateDesc& desc) override;
        void setRasterState(cubos::core::gl::RasterState rs) override;
        cubos::core::gl::DepthStencilState createDepthStencilState(
            const cubos::core::gl::DepthStencilStateDesc& desc) override;
        void setDepthStencilState(cubos::core::gl::DepthStencilState dss) override;
        cubos::core::gl::BlendState createBlendState(const cubos::core::gl::BlendStateDesc& desc) override;
        void setBlendState(cubos::core::gl::BlendState bs) override;
        cubos::core::gl::Sampler createSampler(const cubos::core::gl::SamplerDesc& desc) override;
        cubos::core::gl::Texture1D createTexture1D(const cubos::core::gl::Texture1DDesc& desc) override;
        cubos::core::gl::Texture2D createTexture2D(const cubos::core::gl::Texture2DDesc& desc) override;
        cubos::core::gl::Texture2DArray createTexture2DArray(const cubos::core::gl::Texture2DArrayDesc& desc) override;
        cubos::core::gl::Texture3D createTexture3D(const cubos::core::gl::Texture3DDesc& desc) override;
        cubos::core::gl::CubeMap createCubeMap(const cubos::core::gl::CubeMapDesc& desc) override;
        cubos::core::gl::CubeMapArray createCubeMapArray(const cubos::core::gl::CubeMapArrayDesc& desc) override;
        cubos::core::gl::PixelPackBuffer createPixelPackBuffer(std::size_t size) override;
        cubos::core::gl::ConstantBuffer createConstantBuffer(std::size_t size, const void* data,
                                                             cubos::core::gl::Usage usage) override;
        cubos::core::gl::IndexBuffer createIndexBuffer(std::size_t size, const void* data,
                                                       cubos::core::gl::IndexFormat format,
                                                       cubos::core::gl::Usage usage) override;
        void setIndexBuffer(cubos::core::gl::IndexBuffer ib) override;
        cubos::core::gl::VertexBuffer createVertexBuffer(std::size_t size, const void* data,
                                                         cubos::core::gl::Usage usage) override;
        cubos::core::gl::VertexArray createVertexArray(const cubos::core::gl::VertexArrayDesc& desc) override;
        void setVertexArray(cubos::core::gl::VertexArray va) override;
        cubos::core::gl::ShaderStage createShaderStage(cubos::core::gl::Stage stage, const char* src) override;
        cubos::core::gl::ShaderPipeline createShaderPipeline(cubos::core::gl::ShaderStage vs,
                                                             cubos::core::gl::ShaderStage ps) override;
        cubos::core::gl::ShaderPipeline createShaderPipeline(cubos::core::gl::ShaderStage vs,
                                                             cubos::core::gl::ShaderStage gs,
                                                             cubos::core::gl::ShaderStage ps) override;
        cubos::core::gl::ShaderPipeline createShaderPipeline(cubos::core::gl::ShaderStage cs) override;
        void setShaderPipeline(cubos::core::gl::ShaderPipeline pipeline) override;
        void clearColor(float r, float g, float b, float a) override;
        void clearTargetColor(std::size_t target, float r, float g, float b, float a) override;
        void clearTargetColor(std::size_t target, int r, int g, int b, int a) override;
        void clearDepth(float depth) override;
        void clearStencil(int stencil) override;
        void drawLines(std::size_t offset, std::size_t count) override;
        void drawTriangles(std::size_t offset, std::size_t count) override;
        void drawTrianglesIndexed(std::size_t offset, std::size_t count) override;
        void drawTrianglesInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount) override;
        void drawTrianglesIndexedInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount) override;
        void dispatchCompute(std::size_t x, std::size_t y, std::size_t z) override;
        void memoryBarrier(cubos::core::gl::MemoryBarriers barriers) override;
        void setViewport(int x, int y, int w, int h) override;
        void setScissor(int x, int y, int w, int h) override;
        int getProperty(cubos::core::gl::Property prop) override;

    private:
        RenderDevice& mRenderDevice;
        cubos::core::gl::Framebuffer mCurrentFramebuffer{nullptr};
        cubos::core::gl::Framebuffer mFrontFramebuffer{nullptr};
        cubos::core::gl::Framebuffer mBackFramebuffer{nullptr};
        cubos::core::gl::Texture2D mFrontTexture{nullptr};
        cubos::core::gl::Texture2D mBackTexture{nullptr};
    };
} // namespace tesseratos
