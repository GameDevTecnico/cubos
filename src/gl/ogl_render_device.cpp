#include <cubos/gl/ogl_render_device.hpp>

#ifdef WITH_OPENGL

#include <glad/glad.h>
#ifdef WITH_GLFW
#include <GLFW/glfw3.h>
#endif

using namespace cubos::gl;

OGLRenderDevice::OGLRenderDevice()
{
#ifdef WITH_GLFW
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        ; // TODO: Log critical error and abort
#else
    if (!gladLoadGL())
        ; // TODO: Log critical error and abort
#endif
}

Framebuffer OGLRenderDevice::createFramebuffer(const FramebufferDesc& desc)
{
    return nullptr; // TODO
}

void OGLRenderDevice::setFramebuffer(Framebuffer fb)
{
    // TODO
}

RasterState OGLRenderDevice::createRasterState(const RasterStateDesc& desc)
{
    return nullptr; // TODO
}

void OGLRenderDevice::setRasterState(RasterState rs)
{
    // TODO
}

DepthStencilState OGLRenderDevice::createRasterState(const DepthStencilStateDesc& desc)
{
    return nullptr; // TODO
}

void OGLRenderDevice::setDepthStencilState(DepthStencilState dss)
{
    // TODO
}

BlendState OGLRenderDevice::createBlendState(const BlendStateDesc& desc)
{
    return nullptr; // TODO
}

void OGLRenderDevice::setBlendState(BlendState bs)
{
    // TODO
}

Sampler OGLRenderDevice::createSampler(const SamplerDesc& desc)
{
    return nullptr; // TODO
}

Texture1D OGLRenderDevice::createTexture1D(const Texture1DDesc& desc)
{
    return nullptr; // TODO
}

Texture2D OGLRenderDevice::createTexture2D(const Texture2DDesc& desc)
{
    return nullptr; // TODO
}

Texture3D OGLRenderDevice::createTexture3D(const Texture3DDesc& desc)
{
    return nullptr; // TODO
}

CubeMap OGLRenderDevice::createCubeMap(const CubeMapDesc& desc)
{
    return nullptr; // TODO
}

ConstantBuffer OGLRenderDevice::createConstantBuffer(size_t size, const void* data, Usage usage)
{
    return nullptr; // TODO
}

IndexBuffer OGLRenderDevice::createIndexBuffer(size_t size, const void* data, IndexFormat format, Usage usage)
{
    return nullptr; // TODO
}

void OGLRenderDevice::setIndexBuffer(IndexBuffer ib)
{
    // TODO
}

VertexBuffer OGLRenderDevice::createVertexBuffer(size_t size, const void* data, Usage usage)
{
    return nullptr; // TODO
}

VertexArray OGLRenderDevice::createVertexArray(const VertexArrayDesc& desc)
{
    return nullptr; // TODO
}

void OGLRenderDevice::setVertexArray(VertexArray va)
{
    // TODO
}

ShaderStage OGLRenderDevice::createShaderStage(Stage stage, const char* src)
{
    return nullptr; // TODO
}

ShaderPipeline OGLRenderDevice::createShaderPipeline(ShaderStage vs, ShaderStage ps)
{
    return nullptr; // TODO
}

void OGLRenderDevice::setShaderPipeline(ShaderPipeline pipeline)
{
    // TODO
}

void OGLRenderDevice::clearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void OGLRenderDevice::clearDepth(float depth)
{
    glClearDepth(depth);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void OGLRenderDevice::clearStencil(int stencil)
{
    glClearStencil(stencil);
    glClear(GL_STENCIL_BUFFER_BIT);
}

void OGLRenderDevice::drawTriangles(size_t offset, size_t count)
{
    glDrawArrays(GL_TRIANGLES, offset, count);
}

void OGLRenderDevice::drawTrianglesIndexed(size_t offset, size_t count)
{
    glDrawElements(GL_TRIANGLES, count, this->currentIndexType, reinterpret_cast<const void*>(offset));
}

void OGLRenderDevice::drawTrianglesInstanced(size_t offset, size_t count, size_t instanceCount)
{
    glDrawArraysInstanced(GL_TRIANGLES, offset, count, instanceCount);
}

void OGLRenderDevice::drawTrianglesIndexedInstanced(size_t offset, size_t count, size_t instanceCount)
{
    glDrawElementsInstanced(GL_TRIANGLES, count, this->currentIndexType, reinterpret_cast<const void*>(offset),
                            instanceCount);
}

void OGLRenderDevice::setViewport(int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
}

int OGLRenderDevice::getProperty(Property prop)
{
    switch (prop)
    {
    case Property::MaxAnisotropy:
        if (!GL_ARB_texture_filter_anisotropic)
            return 1;
        else
        {
            GLfloat val;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &val);
            return static_cast<int>(val);
        }

    default:
        return -1;
    }
}

#endif // WITH_OPENGL
