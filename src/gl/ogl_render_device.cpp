#include <cubos/gl/ogl_render_device.hpp>

#ifdef WITH_OPENGL

#include <glad/glad.h>
#ifdef WITH_GLFW
#include <GLFW/glfw3.h>
#endif

#include <cstdlib>
#include <cassert>
#include <list>
#include <string>

using namespace cubos::gl;

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
    // TODO
};

class OGLTexture1D : public impl::Texture1D
{
public:
    virtual void update(size_t x, size_t width, const void* data, size_t level) override
    {
        // TODO
    }

    virtual void generateMipmaps() override
    {
        // TODO
    }

    // TODO
};

class OGLTexture2D : public impl::Texture2D
{
public:
    virtual void update(size_t x, size_t y, size_t width, size_t height, const void* data, size_t level) override
    {
        // TODO
    }

    virtual void generateMipmaps() override
    {
        // TODO
    }

    // TODO
};

class OGLTexture3D : public impl::Texture3D
{
public:
    virtual void update(size_t x, size_t y, size_t z, size_t width, size_t height, size_t depth, const void* data,
                        size_t level) override
    {
        // TODO
    }

    virtual void generateMipmaps() override
    {
        // TODO
    }

    // TODO
};

class OGLCubeMap : public impl::CubeMap
{
public:
    virtual void update(size_t x, size_t y, size_t width, size_t height, const void* data, CubeFace face,
                        size_t level) override
    {
        // TODO
    }

    virtual void generateMipmaps() override
    {
        // TODO
    }

    // TODO
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
    enum class Type
    {
        UniformBlock,
        Sampler1D,
        Sampler2D,
        Sampler3D,
    };

    OGLShaderBindingPoint(const char* name, int loc, Type type) : name(name), loc(loc), type(type)
    {
    }

    virtual void bind(Sampler sampler) override
    {
        // TODO
    }

    virtual void bind(Texture1D tex) override
    {
        // TODO
    }

    virtual void bind(Texture2D tex) override
    {
        // TODO
    }

    virtual void bind(Texture3D tex) override
    {
        // TODO
    }

    virtual void bind(CubeMap cubeMap) override
    {
        // TODO
    }

    virtual void bind(ConstantBuffer cb) override
    {
        assert(this->type == Type::UniformBlock);
        glBindBufferBase(GL_UNIFORM_BUFFER, this->loc, std::static_pointer_cast<OGLConstantBuffer>(cb)->id);
    }

    virtual bool queryConstantBufferStructure(ConstantBufferStructure* structure) override
    {
        return false; // TODO
    }

    std::string name;
    int loc;
    Type type;
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
            bps.emplace_back(name, loc, OGLShaderBindingPoint::Type::UniformBlock);
            return &bps.back();
        }

        // TODO: add samplers

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

        // TODO: log error
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

        // TODO: log error
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

        // TODO: log error
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

            // TODO: log error (couldn't find vertex element with the name {desc.elements[i].name})
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

        // TODO: log error
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

        // TODO: log error
        return nullptr;
    }

    // Check for OpenGL errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        // TODO: log error
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

        // TODO: log error
        return nullptr;
    }

    // Check for OpenGL errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteProgram(id);

        // TODO: log error
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
