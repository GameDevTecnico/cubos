#include <cubos/gl/ogl_render_device.hpp>
#include <cubos/log.hpp>

#ifdef WITH_OPENGL

#include <glad/glad.h>

#ifdef WITH_GLFW
#include <GLFW/glfw3.h>
#endif

#include <cstdlib>
#include <cassert>
#include <list>
#include <string>

using namespace cubos;
using namespace cubos::gl;

// Converts a texture format into the necessary GL parameters
static bool textureFormatToGL(TextureFormat texFormat, GLenum& internalFormat, GLenum& format, GLenum& type)
{
    switch (texFormat)
    {
    case TextureFormat::R8UNorm:
        internalFormat = GL_R8;
        format = GL_R;
        type = GL_UNSIGNED_BYTE;
        break;
    case TextureFormat::R8SNorm:
        internalFormat = GL_R8_SNORM;
        format = GL_R;
        type = GL_BYTE;
        break;
    case TextureFormat::R8UInt:
        internalFormat = GL_R8UI;
        format = GL_RED_INTEGER;
        type = GL_UNSIGNED_BYTE;
        break;
    case TextureFormat::R8SInt:
        internalFormat = GL_R8I;
        format = GL_RED_INTEGER;
        type = GL_BYTE;
        break;
    case TextureFormat::RG8UNorm:
        internalFormat = GL_RG8;
        format = GL_RG;
        type = GL_UNSIGNED_BYTE;
        break;
    case TextureFormat::RG8SNorm:
        internalFormat = GL_RG8_SNORM;
        format = GL_RG;
        type = GL_BYTE;
        break;
    case TextureFormat::RG8UInt:
        internalFormat = GL_RG8UI;
        format = GL_RG_INTEGER;
        type = GL_UNSIGNED_BYTE;
        break;
    case TextureFormat::RG8SInt:
        internalFormat = GL_RG8I;
        format = GL_RG_INTEGER;
        type = GL_BYTE;
        break;
    case TextureFormat::RGBA8UNorm:
        internalFormat = GL_RGBA8;
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        break;
    case TextureFormat::RGBA8SNorm:
        internalFormat = GL_RGBA8_SNORM;
        format = GL_RGBA;
        type = GL_BYTE;
        break;
    case TextureFormat::RGBA8UInt:
        internalFormat = GL_RGBA8UI;
        format = GL_RGBA_INTEGER;
        type = GL_UNSIGNED_BYTE;
        break;
    case TextureFormat::RGBA8SInt:
        internalFormat = GL_RGBA8I;
        format = GL_RGBA_INTEGER;
        type = GL_BYTE;
        break;
    case TextureFormat::R16UNorm:
        internalFormat = GL_R16;
        format = GL_R;
        type = GL_UNSIGNED_SHORT;
        break;
    case TextureFormat::R16SNorm:
        internalFormat = GL_R16_SNORM;
        format = GL_R;
        type = GL_SHORT;
        break;
    case TextureFormat::R16UInt:
        internalFormat = GL_R16UI;
        format = GL_RED_INTEGER;
        type = GL_UNSIGNED_SHORT;
        break;
    case TextureFormat::R16SInt:
        internalFormat = GL_R16I;
        format = GL_RED_INTEGER;
        type = GL_SHORT;
        break;
    case TextureFormat::RG16UNorm:
        internalFormat = GL_RG16;
        format = GL_RG;
        type = GL_UNSIGNED_SHORT;
        break;
    case TextureFormat::RG16SNorm:
        internalFormat = GL_RG16_SNORM;
        format = GL_RG;
        type = GL_SHORT;
        break;
    case TextureFormat::RG16UInt:
        internalFormat = GL_RG16UI;
        format = GL_RG_INTEGER;
        type = GL_UNSIGNED_SHORT;
        break;
    case TextureFormat::RG16SInt:
        internalFormat = GL_RG16I;
        format = GL_RG_INTEGER;
        type = GL_SHORT;
        break;
    case TextureFormat::RGBA16UNorm:
        internalFormat = GL_RGBA16;
        format = GL_RGBA;
        type = GL_UNSIGNED_SHORT;
        break;
    case TextureFormat::RGBA16SNorm:
        internalFormat = GL_RGBA16_SNORM;
        format = GL_RGBA;
        type = GL_SHORT;
        break;
    case TextureFormat::RGBA16UInt:
        internalFormat = GL_RGBA16UI;
        format = GL_RGBA_INTEGER;
        type = GL_UNSIGNED_SHORT;
        break;
    case TextureFormat::RGBA16SInt:
        internalFormat = GL_RGBA16I;
        format = GL_RGBA_INTEGER;
        type = GL_SHORT;
        break;
    case TextureFormat::R32Float:
        internalFormat = GL_R32F;
        format = GL_R;
        type = GL_FLOAT;
        break;
    case TextureFormat::RG32Float:
        internalFormat = GL_RG32F;
        format = GL_RG;
        type = GL_FLOAT;
        break;
    case TextureFormat::RGB32Float:
        internalFormat = GL_RGB32F;
        format = GL_RGB;
        type = GL_FLOAT;
        break;
    case TextureFormat::RGBA32Float:
        internalFormat = GL_RGBA32F;
        format = GL_RGBA;
        type = GL_FLOAT;
        break;
    case TextureFormat::Depth16:
        internalFormat = GL_DEPTH_COMPONENT16;
        format = GL_DEPTH_COMPONENT16;
        type = GL_FLOAT;
        break;
    case TextureFormat::Depth32:
        internalFormat = GL_DEPTH_COMPONENT32F;
        format = GL_DEPTH_COMPONENT32F;
        type = GL_FLOAT;
        break;
    case TextureFormat::Depth24Stencil8:
        internalFormat = GL_DEPTH24_STENCIL8;
        format = GL_DEPTH24_STENCIL8;
        type = GL_FLOAT;
        break;
    case TextureFormat::Depth32Stencil8:
        internalFormat = GL_DEPTH32F_STENCIL8;
        format = GL_DEPTH32F_STENCIL8;
        type = GL_FLOAT;
        break;

    default:
        return false;
    }

    return true;
}

// Converts an addressing mode into the necessary GL parameters
static void addressToGL(AddressMode mode, GLenum& address)
{
    switch (mode)
    {
    case AddressMode::Repeat:
        address = GL_REPEAT;
        break;
    case AddressMode::Mirror:
        address = GL_MIRRORED_REPEAT;
        break;
    case AddressMode::Clamp:
        address = GL_CLAMP;
        break;
    case AddressMode::Border:
        address = GL_CLAMP_TO_BORDER;
        break;
    default:
        abort(); // Invalid addressing mode
    }
}

class OGLFramebuffer : public impl::Framebuffer
{
    // TODO
};

class OGLRasterState : public impl::RasterState
{
    // TODO
};

class OGLDepthStencilState : public impl::DepthStencilState
{
    // TODO
};

class OGLBlendState : public impl::BlendState
{
    // TODO
};

class OGLSampler : public impl::Sampler
{
public:
    OGLSampler(GLuint id) : id(id)
    {
    }

    virtual ~OGLSampler() override
    {
        glDeleteSamplers(1, &this->id);
    }

    GLuint id;
};

class OGLTexture1D : public impl::Texture1D
{
public:
    OGLTexture1D(GLuint id, GLenum internalFormat, GLenum format, GLenum type)
        : id(id), internalFormat(internalFormat), format(format), type(type)
    {
    }

    virtual ~OGLTexture1D() override
    {
        glDeleteTextures(1, &this->id);
    }

    virtual void update(size_t x, size_t width, const void* data, size_t level) override
    {
        glBindTexture(GL_TEXTURE_1D, this->id);
        glTexSubImage1D(GL_TEXTURE_1D, level, x, width, this->format, this->type, data);
    }

    virtual void generateMipmaps() override
    {
        glBindTexture(GL_TEXTURE_1D, this->id);
        glGenerateMipmap(GL_TEXTURE_1D);
    }

    GLuint id;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};

class OGLTexture2D : public impl::Texture2D
{
public:
    OGLTexture2D(GLuint id, GLenum internalFormat, GLenum format, GLenum type)
        : id(id), internalFormat(internalFormat), format(format), type(type)
    {
    }

    virtual ~OGLTexture2D() override
    {
        glDeleteTextures(1, &this->id);
    }

    virtual void update(size_t x, size_t y, size_t width, size_t height, const void* data, size_t level) override
    {
        glBindTexture(GL_TEXTURE_2D, this->id);
        glTexSubImage2D(GL_TEXTURE_2D, level, x, y, width, height, this->format, this->type, data);
    }

    virtual void generateMipmaps() override
    {
        glBindTexture(GL_TEXTURE_2D, this->id);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    GLuint id;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};

class OGLTexture3D : public impl::Texture3D
{
public:
    OGLTexture3D(GLuint id, GLenum internalFormat, GLenum format, GLenum type)
        : id(id), internalFormat(internalFormat), format(format), type(type)
    {
    }

    virtual ~OGLTexture3D() override
    {
        glDeleteTextures(1, &this->id);
    }

    virtual void update(size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, const void* data,
                        size_t level) override
    {
        glBindTexture(GL_TEXTURE_3D, this->id);
        glTexSubImage3D(GL_TEXTURE_3D, level, x, y, z, width, height, depth, this->format, this->type, data);
    }

    virtual void generateMipmaps() override
    {
        glBindTexture(GL_TEXTURE_3D, this->id);
        glGenerateMipmap(GL_TEXTURE_3D);
    }

    GLuint id;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};

class OGLCubeMap : public impl::CubeMap
{
public:
    OGLCubeMap(GLuint id, GLenum internalFormat, GLenum format, GLenum type)
        : id(id), internalFormat(internalFormat), format(format), type(type)
    {
    }

    virtual ~OGLCubeMap() override
    {
        glDeleteTextures(1, &this->id);
    }

    virtual void update(size_t x, size_t y, size_t width, size_t height, const void* data, CubeFace face,
                        size_t level) override
    {
        GLenum glFace;

        switch (face)
        {
        case CubeFace::PositiveX:
            glFace = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
            break;
        case CubeFace::NegativeX:
            glFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
            break;
        case CubeFace::PositiveY:
            glFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
            break;
        case CubeFace::NegativeY:
            glFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
            break;
        case CubeFace::PositiveZ:
            glFace = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
            break;
        case CubeFace::NegativeZ:
            glFace = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
            break;
        default:
            abort();
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);
        glTexSubImage2D(glFace, level, x, y, width, height, this->format, this->type, data);
    }

    virtual void generateMipmaps() override
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    GLuint id;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};

class OGLConstantBuffer : public impl::ConstantBuffer
{
public:
    OGLConstantBuffer(GLuint id) : id(id)
    {
    }

    virtual ~OGLConstantBuffer() override
    {
        glDeleteBuffers(1, &this->id);
    }

    virtual void* map() override
    {
        glBindBuffer(GL_UNIFORM_BUFFER, this->id);
        return glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    }

    virtual void unmap() override
    {
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }

    GLuint id;
};

class OGLIndexBuffer : public impl::IndexBuffer
{
public:
    OGLIndexBuffer(GLuint id, GLenum format) : id(id), format(format)
    {
    }

    virtual ~OGLIndexBuffer() override
    {
        glDeleteBuffers(1, &this->id);
    }

    virtual void* map() override
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id);
        return glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
    }

    virtual void unmap() override
    {
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    }

    GLenum format;
    GLuint id;
};

class OGLVertexBuffer : public impl::VertexBuffer
{
public:
    OGLVertexBuffer(GLuint id) : id(id)
    {
    }

    virtual ~OGLVertexBuffer() override
    {
        glDeleteBuffers(1, &this->id);
    }

    virtual void* map() override
    {
        glBindBuffer(GL_ARRAY_BUFFER, this->id);
        return glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    }

    virtual void unmap() override
    {
        glUnmapBuffer(GL_ARRAY_BUFFER);
    }

    GLuint id;
};

class OGLVertexArray : public impl::VertexArray
{
public:
    OGLVertexArray(GLuint id, const VertexBuffer* buffers) : id(id)
    {
        for (int i = 0; i < CUBOS_GL_MAX_VERTEX_ARRAY_BUFFER_COUNT; ++i)
            this->buffers[i] = buffers[i];
    }

    virtual ~OGLVertexArray() override
    {
        glDeleteVertexArrays(1, &this->id);
    }

    VertexBuffer buffers[CUBOS_GL_MAX_VERTEX_ARRAY_BUFFER_COUNT];
    GLuint id;
};

class OGLShaderStage : public impl::ShaderStage
{
public:
    OGLShaderStage(Stage type, GLuint shader) : type(type), shader(shader)
    {
    }

    virtual ~OGLShaderStage() override
    {
        glDeleteShader(this->shader);
    }

    virtual Stage getType() override
    {
        return this->type;
    }

    Stage type;
    GLuint shader;
};

class OGLShaderBindingPoint : public impl::ShaderBindingPoint
{
public:
    OGLShaderBindingPoint(const char* name, int loc) : name(name), loc(loc)
    {
    }

    virtual void bind(Sampler sampler) override
    {
        if (sampler)
            glBindSampler(this->loc, std::static_pointer_cast<OGLSampler>(sampler)->id);
        else
            glBindSampler(this->loc, 0);
    }

    virtual void bind(Texture1D tex) override
    {
        glActiveTexture(GL_TEXTURE0 + this->loc);
        if (tex)
            glBindTexture(GL_TEXTURE_1D, std::static_pointer_cast<OGLTexture1D>(tex)->id);
        else
            glBindTexture(GL_TEXTURE_1D, 0);
        glUniform1i(this->loc, this->loc);
    }

    virtual void bind(Texture2D tex) override
    {
        glActiveTexture(GL_TEXTURE0 + this->loc);
        if (tex)
            glBindTexture(GL_TEXTURE_2D, std::static_pointer_cast<OGLTexture2D>(tex)->id);
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(this->loc, this->loc);
    }

    virtual void bind(Texture3D tex) override
    {
        glActiveTexture(GL_TEXTURE0 + this->loc);
        if (tex)
            glBindTexture(GL_TEXTURE_3D, std::static_pointer_cast<OGLTexture3D>(tex)->id);
        else
            glBindTexture(GL_TEXTURE_3D, 0);
        glUniform1i(this->loc, this->loc);
    }

    virtual void bind(CubeMap cubeMap) override
    {
        glActiveTexture(GL_TEXTURE0 + this->loc);
        if (cubeMap)
            glBindTexture(GL_TEXTURE_CUBE_MAP, std::static_pointer_cast<OGLCubeMap>(cubeMap)->id);
        else
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glUniform1i(this->loc, this->loc);
    }

    virtual void bind(ConstantBuffer cb) override
    {
        if (cb)
            glBindBufferBase(GL_UNIFORM_BUFFER, this->loc, std::static_pointer_cast<OGLConstantBuffer>(cb)->id);
        else
            glBindBufferBase(GL_UNIFORM_BUFFER, this->loc, 0);
    }

    virtual bool queryConstantBufferStructure(ConstantBufferStructure* structure) override
    {
        return false; // TODO
    }

    std::string name;
    int loc;
};

class OGLShaderPipeline : public impl::ShaderPipeline
{
public:
    OGLShaderPipeline(ShaderStage vs, ShaderStage ps, GLuint program) : vs(vs), ps(ps), program(program)
    {
    }

    virtual ~OGLShaderPipeline() override
    {
        glDeleteProgram(this->program);
    }

    virtual ShaderBindingPoint getBindingPoint(const char* name) override
    {
        // Search for already existing binding point
        for (auto& bp : this->bps)
            if (bp.name == name)
                return &bp;

        // No existing binding point was found, it must be created first
        // Search for uniform block binding
        auto loc = glGetUniformBlockIndex(this->program, name);
        if (loc != GL_INVALID_INDEX)
        {
            glUniformBlockBinding(this->program, loc, loc);
            bps.emplace_back(name, loc);
            return &bps.back();
        }

        loc = glGetUniformLocation(this->program, name);
        if (loc != -1)
        {
            bps.emplace_back(name, loc);
            return &bps.back();
        }

        return nullptr;
    }

    ShaderStage vs, ps;
    GLuint program;
    std::list<OGLShaderBindingPoint> bps;
};

OGLRenderDevice::OGLRenderDevice()
{
#ifdef WITH_GLFW
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        logCritical("OGLRenderDevice::OGLRenderDevice() failed: OpenGL loader failed");
        abort();
    }
#else
    if (!gladLoadGL())
    {
        logCritical("OGLRenderDevice::OGLRenderDevice() failed: OpenGL loader failed");
        abort();
    }
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
    GLenum addressU, addressV, addressW;

    addressToGL(desc.addressU, addressU);
    addressToGL(desc.addressV, addressV);
    addressToGL(desc.addressW, addressW);

    GLenum minFilter, magFilter;

    switch (desc.minFilter)
    {
    case TextureFilter::Nearest:
        if (desc.mipmapFilter == TextureFilter::None)
            minFilter = GL_NEAREST;
        else if (desc.mipmapFilter == TextureFilter::Nearest)
            minFilter = GL_NEAREST_MIPMAP_NEAREST;
        else if (desc.mipmapFilter == TextureFilter::Linear)
            minFilter = GL_NEAREST_MIPMAP_LINEAR;
        else
            abort(); // Invalid enum value
        break;
    case TextureFilter::Linear:
        if (desc.mipmapFilter == TextureFilter::None)
            minFilter = GL_LINEAR;
        else if (desc.mipmapFilter == TextureFilter::Nearest)
            minFilter = GL_LINEAR_MIPMAP_NEAREST;
        else if (desc.mipmapFilter == TextureFilter::Linear)
            minFilter = GL_LINEAR_MIPMAP_LINEAR;
        else
            abort(); // Invalid enum value
        break;
    default:
        abort(); // Invalid enum value
    }

    switch (desc.magFilter)
    {
    case TextureFilter::Nearest:
        magFilter = GL_NEAREST;
        break;
    case TextureFilter::Linear:
        magFilter = GL_LINEAR;
        break;
    default:
        abort(); // Invalid enum value
    }

    // Initialize sampler
    GLuint id;
    glGenSamplers(1, &id);
    glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, minFilter);
    glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, magFilter);
    if (GL_ARB_texture_filter_anisotropic)
        glSamplerParameteri(id, GL_TEXTURE_MAX_ANISOTROPY, desc.maxAnisotropy);
    glSamplerParameteri(id, GL_TEXTURE_WRAP_S, addressU);
    glSamplerParameteri(id, GL_TEXTURE_WRAP_T, addressV);
    glSamplerParameteri(id, GL_TEXTURE_WRAP_R, addressW);
    glSamplerParameterfv(id, GL_TEXTURE_BORDER_COLOR, desc.borderColor);

    // Check errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteSamplers(1, &id);
        logError("OGLRenderDevice::createSampler() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLSampler>(id);
}

Texture1D OGLRenderDevice::createTexture1D(const Texture1DDesc& desc)
{
    if (desc.format == TextureFormat::Depth16 || desc.format == TextureFormat::Depth32 ||
        desc.format == TextureFormat::Depth24Stencil8 || desc.format == TextureFormat::Depth32Stencil8)
    {
        logError("OGLRenderDevice::createTexture1D() failed: depth/stencil formats are not supported on 1D textures");
        return nullptr;
    }

    GLenum internalFormat, format, type;

    if (!textureFormatToGL(desc.format, internalFormat, format, type))
    {
        logError("OGLRenderDevice::createTexture1D() failed: unsupported texture format {}", desc.format);
        return nullptr;
    }

    // Initialize texture
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_1D, id);
    for (size_t i = 0, div = 1; i < desc.mipLevelCount; ++i, div *= 2)
        glTexImage1D(GL_TEXTURE_1D, i, internalFormat, desc.width / div, 0, format, type, desc.data[i]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    if (GL_ARB_texture_filter_anisotropic)
        glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);

    // Check errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteTextures(1, &id);
        logError("OGLRenderDevice::createTexture1D() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLTexture1D>(id, internalFormat, format, type);
}

Texture2D OGLRenderDevice::createTexture2D(const Texture2DDesc& desc)
{
    GLenum internalFormat, format, type;

    if (!textureFormatToGL(desc.format, internalFormat, format, type))
    {
        logError("OGLRenderDevice::createTexture2D() failed: unsupported texture format {}", desc.format);
        return nullptr;
    }

    // Initialize texture
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    for (size_t i = 0, div = 1; i < desc.mipLevelCount; ++i, div *= 2)
        glTexImage2D(GL_TEXTURE_2D, i, internalFormat, desc.width / div, desc.height / div, 0, format, type,
                     desc.data[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (GL_ARB_texture_filter_anisotropic)
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);

    // Check errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteTextures(1, &id);
        logError("OGLRenderDevice::createTexture2D() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLTexture2D>(id, internalFormat, format, type);
}

Texture3D OGLRenderDevice::createTexture3D(const Texture3DDesc& desc)
{
    if (desc.format == TextureFormat::Depth16 || desc.format == TextureFormat::Depth32 ||
        desc.format == TextureFormat::Depth24Stencil8 || desc.format == TextureFormat::Depth32Stencil8)
    {
        logError("OGLRenderDevice::createTexture3D() failed: depth/stencil formats are not supported on 3D textures");
        return nullptr;
    }

    GLenum internalFormat, format, type;

    if (!textureFormatToGL(desc.format, internalFormat, format, type))
    {
        logError("OGLRenderDevice::createTexture3D() failed: unsupported texture format {}", desc.format);
        return nullptr;
    }

    // Initialize texture
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_3D, id);
    for (size_t i = 0, div = 1; i < desc.mipLevelCount; ++i, div *= 2)
        glTexImage3D(GL_TEXTURE_3D, i, internalFormat, desc.width / div, desc.height / div, desc.depth / div, 0, format,
                     type, desc.data[i]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    if (GL_ARB_texture_filter_anisotropic)
        glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);

    // Check errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteTextures(1, &id);
        logError("OGLRenderDevice::createTexture3D() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLTexture3D>(id, internalFormat, format, type);
}

CubeMap OGLRenderDevice::createCubeMap(const CubeMapDesc& desc)
{
    GLenum internalFormat, format, type;

    if (!textureFormatToGL(desc.format, internalFormat, format, type))
    {
        logError("OGLRenderDevice::createCubeMap() failed: unsupported texture format {}", desc.format);
        return nullptr;
    }

    // Initialize texture
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    for (size_t i = 0, div = 1; i < desc.mipLevelCount; ++i, div *= 2)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, i, internalFormat, desc.width / div, desc.height / div, 0, format,
                     type, desc.data[static_cast<int>(CubeFace::PositiveX)][i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, i, internalFormat, desc.width / div, desc.height / div, 0, format,
                     type, desc.data[static_cast<int>(CubeFace::NegativeX)][i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, i, internalFormat, desc.width / div, desc.height / div, 0, format,
                     type, desc.data[static_cast<int>(CubeFace::PositiveY)][i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, i, internalFormat, desc.width / div, desc.height / div, 0, format,
                     type, desc.data[static_cast<int>(CubeFace::NegativeY)][i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, i, internalFormat, desc.width / div, desc.height / div, 0, format,
                     type, desc.data[static_cast<int>(CubeFace::PositiveZ)][i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, i, internalFormat, desc.width / div, desc.height / div, 0, format,
                     type, desc.data[static_cast<int>(CubeFace::NegativeZ)][i]);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (GL_ARB_texture_filter_anisotropic)
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);

    // Check errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteTextures(1, &id);
        logError("OGLRenderDevice::createCubeMap() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLCubeMap>(id, internalFormat, format, type);
}

ConstantBuffer OGLRenderDevice::createConstantBuffer(size_t size, const void* data, Usage usage)
{
    // Validate arguments
    if (usage == Usage::Static && data == nullptr)
        abort();

    GLenum glUsage;
    if (usage == Usage::Default)
        glUsage = GL_STATIC_DRAW;
    else if (usage == Usage::Static)
        glUsage = GL_STATIC_DRAW;
    else if (usage == Usage::Dynamic)
        glUsage = GL_DYNAMIC_DRAW;
    else
        abort(); // Invalid enum value

    // Initialize buffer
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, size, data, glUsage);

    // Check errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteBuffers(1, &id);
        logError("OGLRenderDevice::createConstantBuffer() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLConstantBuffer>(id);
}

IndexBuffer OGLRenderDevice::createIndexBuffer(size_t size, const void* data, IndexFormat format, Usage usage)
{
    // Validate arguments
    if (usage == Usage::Static && data == nullptr)
        abort();

    GLenum glUsage;
    if (usage == Usage::Default)
        glUsage = GL_STATIC_DRAW;
    else if (usage == Usage::Static)
        glUsage = GL_STATIC_DRAW;
    else if (usage == Usage::Dynamic)
        glUsage = GL_DYNAMIC_DRAW;
    else
        abort(); // Invalid enum value

    GLenum glFormat;
    if (format == IndexFormat::UShort)
        glFormat = GL_UNSIGNED_SHORT;
    else if (format == IndexFormat::UInt)
        glFormat = GL_UNSIGNED_INT;
    else
        abort(); // Invalid enum value

    // Initialize buffer
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, glUsage);

    // Check errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteBuffers(1, &id);
        logError("OGLRenderDevice::createIndexBuffer() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLIndexBuffer>(id, glFormat);
}

void OGLRenderDevice::setIndexBuffer(IndexBuffer _ib)
{
    auto ib = std::static_pointer_cast<OGLIndexBuffer>(_ib);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->id);
    this->currentIndexFormat = ib->format;
}

VertexBuffer OGLRenderDevice::createVertexBuffer(size_t size, const void* data, Usage usage)
{
    // Validate arguments
    if (usage == Usage::Static && data == nullptr)
        abort();

    GLenum glUsage;
    if (usage == Usage::Default)
        glUsage = GL_STATIC_DRAW;
    else if (usage == Usage::Static)
        glUsage = GL_STATIC_DRAW;
    else if (usage == Usage::Dynamic)
        glUsage = GL_DYNAMIC_DRAW;
    else
        abort(); // Invalid enum value

    // Initialize buffer
    GLuint id;
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, glUsage);

    // Check errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteBuffers(1, &id);
        logError("OGLRenderDevice::createVertexBuffer() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLVertexBuffer>(id);
}

VertexArray OGLRenderDevice::createVertexArray(const VertexArrayDesc& desc)
{
    auto pp = std::static_pointer_cast<OGLShaderPipeline>(desc.shaderPipeline);
    assert(pp != nullptr);

    // Initialize vertex array
    GLuint id;
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    // Link elements
    assert(desc.elementCount <= CUBOS_GL_MAX_VERTEX_ARRAY_ELEMENT_COUNT);
    for (int i = 0; i < desc.elementCount; ++i)
    {
        // Get buffer
        auto vb = std::static_pointer_cast<OGLVertexBuffer>(desc.buffers[desc.elements[i].buffer.index]);
        assert(vb != nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, vb->id);

        // Get attribute location
        GLint loc = glGetAttribLocation(pp->program, desc.elements[i].name);
        if (loc == -1)
        {
            glDeleteVertexArrays(1, &id);
            logError("OGLRenderDevice::createVertexArray() failed: couldn't find vertex element with name '{}'", desc.elements[i].name);
            return nullptr;
        }

        // Get type
        GLenum type;
        bool normalized;
        bool integer;

        switch (desc.elements[i].type)
        {
        case Type::Byte:
            type = GL_BYTE;
            normalized = GL_FALSE;
            integer = true;
            break;
        case Type::Short:
            type = GL_SHORT;
            normalized = GL_FALSE;
            integer = true;
            break;
        case Type::Int:
            type = GL_INT;
            normalized = GL_FALSE;
            integer = true;
            break;
        case Type::UByte:
            type = GL_UNSIGNED_BYTE;
            normalized = GL_FALSE;
            integer = true;
            break;
        case Type::UShort:
            type = GL_UNSIGNED_SHORT;
            normalized = GL_FALSE;
            integer = true;
            break;
        case Type::UInt:
            type = GL_UNSIGNED_INT;
            normalized = GL_FALSE;
            integer = true;
            break;
        case Type::NByte:
            type = GL_BYTE;
            normalized = GL_TRUE;
            integer = false;
            break;
        case Type::NShort:
            type = GL_SHORT;
            normalized = GL_TRUE;
            integer = false;
            break;
        case Type::NUByte:
            type = GL_UNSIGNED_BYTE;
            normalized = GL_TRUE;
            integer = false;
            break;
        case Type::NUShort:
            type = GL_UNSIGNED_SHORT;
            normalized = GL_TRUE;
            integer = false;
            break;
        case Type::Float:
            type = GL_FLOAT;
            normalized = GL_FALSE;
            integer = false;
            break;
        default:
            abort(); // Invalid vertex element type
        }

        assert(desc.elements[i].size >= 1 && desc.elements[i].size <= 4);

        glEnableVertexAttribArray(loc);
        if (!integer)
            glVertexAttribPointer(loc, (GLint)desc.elements[i].size, type, normalized,
                                  (GLsizei)desc.elements[i].buffer.stride, (const void*)desc.elements[i].buffer.offset);
        else
            glVertexAttribIPointer(loc, (GLint)desc.elements[i].size, type, (GLsizei)desc.elements[i].buffer.stride,
                                   (const void*)desc.elements[i].buffer.offset);
    }

    // Check errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteVertexArrays(1, &id);
        logError("OGLRenderDevice::createVertexArray() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLVertexArray>(id, desc.buffers);
}

void OGLRenderDevice::setVertexArray(VertexArray va)
{
    glBindVertexArray(std::static_pointer_cast<OGLVertexArray>(va)->id);
}

ShaderStage OGLRenderDevice::createShaderStage(Stage stage, const char* src)
{
    GLenum shader_type;
    switch (stage)
    {
    case Stage::Vertex:
        shader_type = GL_VERTEX_SHADER;
        break;
    case Stage::Pixel:
        shader_type = GL_FRAGMENT_SHADER;
        break;
    }

    // Initialize shader
    GLuint id = glCreateShader(shader_type);
    glShaderSource(id, 1, (const GLchar* const*)&src, NULL);
    glCompileShader(id);

    // Check for errors
    GLint success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(id, sizeof(infoLog), NULL, infoLog);
        glDeleteShader(id);
        logError("OGLRenderDevice::createShaderStage() failed: shader compilation failed, info log: {}", infoLog);
        return nullptr;
    }

    // Check for OpenGL errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        logError("OGLRenderDevice::createShaderStage() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLShaderStage>(stage, id);
}

ShaderPipeline OGLRenderDevice::createShaderPipeline(ShaderStage _vs, ShaderStage _ps)
{
    auto vs = std::static_pointer_cast<OGLShaderStage>(_vs);
    auto ps = std::static_pointer_cast<OGLShaderStage>(_ps);

    // Initialize program
    auto id = glCreateProgram();
    glAttachShader(id, vs->shader);
    glAttachShader(id, ps->shader);
    glLinkProgram(id);

    // Check for linking errors
    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(id, sizeof(infoLog), NULL, infoLog);
        glDeleteProgram(id);
        logError("OGLRenderDevice::createShaderPipeline() failed: program linking failed, info log: {}", infoLog);
        return nullptr;
    }

    // Check for OpenGL errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteProgram(id);
        logError("OGLRenderDevice::createShaderPipeline() failed: OpenGL error {}", glErr);
        return nullptr;
    }

    return std::make_shared<OGLShaderPipeline>(vs, ps, id);
}

void OGLRenderDevice::setShaderPipeline(ShaderPipeline pipeline)
{
    glUseProgram(std::static_pointer_cast<OGLShaderPipeline>(pipeline)->program);
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
    glDrawElements(GL_TRIANGLES, count, this->currentIndexFormat, reinterpret_cast<const void*>(offset));
}

void OGLRenderDevice::drawTrianglesInstanced(size_t offset, size_t count, size_t instanceCount)
{
    glDrawArraysInstanced(GL_TRIANGLES, offset, count, instanceCount);
}

void OGLRenderDevice::drawTrianglesIndexedInstanced(size_t offset, size_t count, size_t instanceCount)
{
    glDrawElementsInstanced(GL_TRIANGLES, count, this->currentIndexFormat, reinterpret_cast<const void*>(offset),
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
