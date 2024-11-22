/**
 * OpenGL 3.3 Reference http://opengl.org/sdk/docs/man3/
 * OpenGL ES 3.0 Reference https://registry.khronos.org/OpenGL-Refpages/es3.0/
 * Differences Between WebGL and OpenGL ES 3.0 https://registry.khronos.org/webgl/specs/latest/2.0/#5
 */

#include <cassert>
#include <cstdlib>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include <glad/glad.h>

#include <cubos/core/reflection/external/cstring.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/tel/logging.hpp>

#include "ogl_render_device.hpp"

using namespace cubos::core;
using namespace cubos::core::gl;

#define LOG_GL_ERROR_SRC(src, err) CUBOS_ERROR("{}: {}", src, err)
#define LOG_GL_ERROR(err) LOG_GL_ERROR_SRC("OpenGL error", err)

#define CHECK_GL_ERROR(...)                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        GLenum glErr = glGetError();                                                                                   \
        if (glErr != GL_NO_ERROR)                                                                                      \
        {                                                                                                              \
            LOG_GL_ERROR_SRC(#__VA_ARGS__, glErr);                                                                     \
        }                                                                                                              \
    } while (false)

#define CLEANUP_GL_ERROR_RET(ret, ...)                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        GLenum glErr = glGetError();                                                                                   \
        if (glErr != GL_NO_ERROR)                                                                                      \
        {                                                                                                              \
            __VA_ARGS__;                                                                                               \
            LOG_GL_ERROR(glErr);                                                                                       \
            return ret;                                                                                                \
        }                                                                                                              \
    } while (false)

#ifndef NDEBUG // FIXME: This should be a build option, to allow Release with error checking
#define CHECK(...)                                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        __VA_ARGS__;                                                                                                   \
        CHECK_GL_ERROR(__VA_ARGS__);                                                                                   \
    } while (false)

#define CHECK_VAL(val, ...)                                                                                            \
    do                                                                                                                 \
    {                                                                                                                  \
        val = __VA_ARGS__;                                                                                             \
        CHECK_GL_ERROR(__VA_ARGS__);                                                                                   \
    } while (false)
#else
#define CHECK(...) __VA_ARGS__
#define CHECK_VAL(val, ...) val = __VA_ARGS__
#endif

#ifndef __EMSCRIPTEN__
// OpenGL message callback (errors, warnings, etc)
static void GLAPIENTRY messageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                       const GLchar* message, const void* userParam)
{
    (void)length;
    (void)userParam;
    (void)id;

    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        CUBOS_ERROR("OpenGL error (source = {}, type = {}): {}", source, type, message);
    }
    else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
    {
        CUBOS_WARN("OpenGL warning (source = {}, type = {}): {}", source, type, message);
    }
}
#endif // __EMSCRIPTEN__

// Converts a texture format into the necessary GL parameters
static bool textureFormatToGL(TextureFormat texFormat, GLenum& internalFormat, GLenum& format, GLenum& type)
{
    switch (texFormat)
    {
    case TextureFormat::R8UNorm:
        internalFormat = GL_R8;
        format = GL_RED;
        type = GL_UNSIGNED_BYTE;
        break;
    case TextureFormat::R8SNorm:
        internalFormat = GL_R8_SNORM;
        format = GL_RED;
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
        internalFormat = GL_RGBA;
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
        internalFormat = GL_R16F;
        format = GL_RED;
        type = GL_UNSIGNED_SHORT;
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
        internalFormat = GL_RG16F;
        format = GL_RG;
        type = GL_UNSIGNED_SHORT;
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
        internalFormat = GL_RGBA16F;
        format = GL_RGBA;
        type = GL_UNSIGNED_SHORT;
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
    case TextureFormat::R16Float:
        internalFormat = GL_R16F;
        format = GL_RED;
        type = GL_FLOAT;
        break;
    case TextureFormat::R32Float:
        internalFormat = GL_R32F;
        format = GL_RED;
        type = GL_FLOAT;
        break;
    case TextureFormat::RG16Float:
        internalFormat = GL_RG16F;
        format = GL_RG;
        type = GL_FLOAT;
        break;
    case TextureFormat::RG32Float:
        internalFormat = GL_RG32F;
        format = GL_RG;
        type = GL_FLOAT;
        break;
    case TextureFormat::RGB16Float:
        internalFormat = GL_RGBA16F;
        format = GL_RGB;
        type = GL_FLOAT;
        break;
    case TextureFormat::RGB32Float:
        internalFormat = GL_RGB32F;
        format = GL_RGB;
        type = GL_FLOAT;
        break;
    case TextureFormat::RGBA16Float:
        internalFormat = GL_RGBA16F;
        format = GL_RGBA;
        type = GL_FLOAT;
        break;
    case TextureFormat::RGBA32Float:
        internalFormat = GL_RGBA32F;
        format = GL_RGBA;
        type = GL_FLOAT;
        break;
    case TextureFormat::Depth16:
        internalFormat = GL_DEPTH_COMPONENT16;
        format = GL_DEPTH_COMPONENT;
        type = GL_FLOAT;
        break;
    case TextureFormat::Depth32:
        internalFormat = GL_DEPTH_COMPONENT32F;
        format = GL_DEPTH_COMPONENT;
        type = GL_FLOAT;
        break;
    case TextureFormat::Depth24Stencil8:
        internalFormat = GL_DEPTH24_STENCIL8;
        format = GL_DEPTH_STENCIL;
        type = GL_UNSIGNED_INT_24_8;
        break;
    case TextureFormat::Depth32Stencil8:
        internalFormat = GL_DEPTH32F_STENCIL8;
        format = GL_DEPTH_STENCIL;
        type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        break;

    default:
        return false;
    }

    return true;
}

// Figures out the GL format and type which should be used with glReadPixels
static void textureFormatToGLForCopying(TextureFormat texFormat, GLenum& format, GLenum& type)
{
    switch (texFormat)
    {
    case TextureFormat::R8UNorm:
    case TextureFormat::RG8UNorm:
    case TextureFormat::RGBA8UNorm:
    case TextureFormat::R8SNorm:
    case TextureFormat::RG8SNorm:
    case TextureFormat::RGBA8SNorm:
        format = GL_RGBA;
        type = GL_UNSIGNED_BYTE;
        break;

    case TextureFormat::R8UInt:
    case TextureFormat::RG8UInt:
    case TextureFormat::RGBA8UInt:
    case TextureFormat::R16UInt:
    case TextureFormat::RG16UInt:
    case TextureFormat::RGBA16UInt:
        format = GL_RGBA_INTEGER;
        type = GL_UNSIGNED_INT;
        break;

    case TextureFormat::R8SInt:
    case TextureFormat::RG8SInt:
    case TextureFormat::RGBA8SInt:
    case TextureFormat::R16SInt:
    case TextureFormat::RG16SInt:
    case TextureFormat::RGBA16SInt:
        format = GL_RGBA_INTEGER;
        type = GL_INT;
        break;

    case TextureFormat::R16Float:
    case TextureFormat::RG16Float:
    case TextureFormat::RGBA16Float:
    case TextureFormat::R32Float:
    case TextureFormat::RG32Float:
    case TextureFormat::RGBA32Float:
        format = GL_RGBA;
        type = GL_FLOAT;
        break;

    default:
        break;
    }
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
        address = GL_CLAMP_TO_EDGE;
        break;
    default:
        CUBOS_FAIL("Invalid addressing mode");
    }
}

// Converts a cube face to a GL face
static void cubeFaceToGL(CubeFace cubeFace, GLenum& face)
{
    switch (cubeFace)
    {
    case CubeFace::PositiveX:
        face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        break;
    case CubeFace::NegativeX:
        face = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        break;
    case CubeFace::PositiveY:
        face = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        break;
    case CubeFace::NegativeY:
        face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        break;
    case CubeFace::PositiveZ:
        face = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        break;
    case CubeFace::NegativeZ:
        face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        break;
    default:
        CUBOS_FAIL("Invalid enum");
    }
}

// Converts a face to a GL face
static void faceToGL(Face face, GLenum& glFace)
{
    switch (face)
    {
    case Face::Front:
        glFace = GL_FRONT;
        break;
    case Face::Back:
        glFace = GL_BACK;
        break;
    case Face::FrontAndBack:
        glFace = GL_FRONT_AND_BACK;
        break;
    default:
        CUBOS_FAIL("Invalid enum");
    }
}

// Converts a winding to a GL winding
static void windingToGL(Winding winding, GLenum& glWinding)
{
    switch (winding)
    {
    case Winding::CW:
        glWinding = GL_CW;
        break;
    case Winding::CCW:
        glWinding = GL_CCW;
        break;
    default:
        CUBOS_FAIL("Invalid enum");
    }
}

// Converts a comparison function to the GL equivalent
static void compareToGL(Compare compare, GLenum& glCompare)
{
    switch (compare)
    {
    case Compare::Never:
        glCompare = GL_NEVER;
        break;
    case Compare::Less:
        glCompare = GL_LESS;
        break;
    case Compare::LEqual:
        glCompare = GL_LEQUAL;
        break;
    case Compare::Greater:
        glCompare = GL_GREATER;
        break;
    case Compare::GEqual:
        glCompare = GL_GEQUAL;
        break;
    case Compare::Equal:
        glCompare = GL_EQUAL;
        break;
    case Compare::NEqual:
        glCompare = GL_NOTEQUAL;
        break;
    case Compare::Always:
        glCompare = GL_ALWAYS;
        break;
    default:
        CUBOS_FAIL("Invalid enum");
    }
}

// Converts a stencil action to the GL equivalent
static void stencilActionToGL(StencilAction action, GLenum& glAction)
{
    switch (action)
    {
    case StencilAction::Keep:
        glAction = GL_NEVER;
        break;
    case StencilAction::Zero:
        glAction = GL_ZERO;
        break;
    case StencilAction::Replace:
        glAction = GL_REPLACE;
        break;
    case StencilAction::Increment:
        glAction = GL_INCR;
        break;
    case StencilAction::IncrementWrap:
        glAction = GL_INCR_WRAP;
        break;
    case StencilAction::Decrement:
        glAction = GL_DECR;
        break;
    case StencilAction::DecrementWrap:
        glAction = GL_DECR_WRAP;
        break;
    case StencilAction::Invert:
        glAction = GL_INVERT;
        break;
    default:
        CUBOS_FAIL("Invalid enum");
    }
}

// Converts a blend factor to the GL equivalent
static void blendFactorToGL(BlendFactor blendFactor, GLenum& glBlendFactor)
{
    switch (blendFactor)
    {
    case BlendFactor::Zero:
        glBlendFactor = GL_ZERO;
        break;
    case BlendFactor::One:
        glBlendFactor = GL_ONE;
        break;
    case BlendFactor::SrcColor:
        glBlendFactor = GL_SRC_COLOR;
        break;
    case BlendFactor::InvSrcColor:
        glBlendFactor = GL_ONE_MINUS_SRC_COLOR;
        break;
    case BlendFactor::DstColor:
        glBlendFactor = GL_DST_COLOR;
        break;
    case BlendFactor::InvDstColor:
        glBlendFactor = GL_ONE_MINUS_DST_COLOR;
        break;
    case BlendFactor::SrcAlpha:
        glBlendFactor = GL_SRC_ALPHA;
        break;
    case BlendFactor::InvSrcAlpha:
        glBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
        break;
    case BlendFactor::DstAlpha:
        glBlendFactor = GL_DST_ALPHA;
        break;
    case BlendFactor::InvDstAlpha:
        glBlendFactor = GL_ONE_MINUS_DST_ALPHA;
        break;
    default:
        CUBOS_FAIL("Invalid enum");
    }
}

// Converts a blend operation to the GL equivalent
static void blendOpToGL(BlendOp blendOp, GLenum& glBlendOp)
{
    switch (blendOp)
    {
    case BlendOp::Add:
        glBlendOp = GL_FUNC_ADD;
        break;
    case BlendOp::Substract:
        glBlendOp = GL_FUNC_SUBTRACT;
        break;
    case BlendOp::RevSubstract:
        glBlendOp = GL_FUNC_REVERSE_SUBTRACT;
        break;
    case BlendOp::Max:
        glBlendOp = GL_MAX;
        break;
    case BlendOp::Min:
        glBlendOp = GL_MIN;
        break;
    default:
        CUBOS_FAIL("Invalid enum");
    }
}

class OGLFramebuffer : public impl::Framebuffer
{
public:
    OGLFramebuffer(std::shared_ptr<bool> destroyed, GLuint id)
        : destroyed(std::move(destroyed))
        , id(id)
    {
    }

    ~OGLFramebuffer() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteFramebuffers(1, &this->id));
        }
    }

    std::shared_ptr<bool> destroyed;

    GLuint id;
};

class OGLRasterState : public impl::RasterState
{
public:
    GLboolean cullEnabled;
    GLboolean scissorEnabled;
    GLenum frontFace;
    GLenum cullFace;
    GLenum polygonMode;
};

class OGLDepthStencilState : public impl::DepthStencilState
{
public:
    GLboolean depthEnabled;
    GLboolean depthWriteEnabled;
    GLfloat depthNear;
    GLfloat depthFar;
    GLenum depthFunc;

    GLuint stencilRef;
    GLboolean stencilEnabled;
    GLuint stencilReadMask;
    GLuint stencilWriteMask;

    GLenum frontStencilFunc;
    GLenum frontFaceStencilFail;
    GLenum frontFaceStencilPass;
    GLenum frontFaceDepthFail;

    GLenum backStencilFunc;
    GLenum backFaceStencilFail;
    GLenum backFaceStencilPass;
    GLenum backFaceDepthFail;
};

class OGLBlendState : public impl::BlendState
{
public:
    GLboolean blendEnabled;
    GLenum srcFactor;
    GLenum dstFactor;
    GLenum blendOp;
    GLenum srcAlphaFactor;
    GLenum dstAlphaFactor;
    GLenum alphaBlendOp;
};

class OGLPixelPackBuffer : public impl::PixelPackBuffer
{
public:
    OGLPixelPackBuffer(std::shared_ptr<bool> destroyed, GLuint id, GLsizeiptr size)
        : destroyed(std::move(destroyed))
        , id(id)
        , size(size)
    {
    }

    ~OGLPixelPackBuffer() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteBuffers(1, &this->id));
        }
    }

    void copyTo(void* data, std::size_t offset, std::size_t size) override
    {
        CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, this->id));
        CHECK(glGetBufferSubData(GL_PIXEL_PACK_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size),
                                 data));
        CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
    }

    std::shared_ptr<bool> destroyed;

    GLuint id;
    GLsizeiptr size;
};

class OGLSampler : public impl::Sampler
{
public:
    OGLSampler(std::shared_ptr<bool> destroyed, GLuint id)
        : destroyed(std::move(destroyed))
        , id(id)
    {
    }

    ~OGLSampler() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteSamplers(1, &this->id));
        }
    }

    std::shared_ptr<bool> destroyed;

    GLuint id;
};

class OGLTexture2D : public impl::Texture2D
{
public:
    OGLTexture2D(std::shared_ptr<bool> destroyed, GLuint id, GLenum internalFormat, GLenum format, GLenum type,
                 GLenum copyToFormat, GLenum copyToType, GLsizei width, GLsizei height)
        : destroyed(std::move(destroyed))
        , id(id)
        , internalFormat(internalFormat)
        , format(format)
        , type(type)
        , copyToFormat(copyToFormat)
        , copyToType(copyToType)
        , width(width)
        , height(height)
    {
    }

    ~OGLTexture2D() override
    {
        if (!*destroyed)
        {
            if (this->framebuffer != 0)
            {
                CHECK(glDeleteFramebuffers(1, &this->framebuffer));
            }

            CHECK(glDeleteTextures(1, &this->id));
        }
    }

    void update(std::size_t x, std::size_t y, std::size_t width, std::size_t height, const void* data,
                std::size_t level) override
    {
        CHECK(glBindTexture(GL_TEXTURE_2D, this->id));
        CHECK(glTexSubImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), static_cast<GLint>(x), static_cast<GLint>(y),
                              static_cast<GLsizei>(width), static_cast<GLsizei>(height), this->format, this->type,
                              data));
    }

    void read(void* outputBuffer) override
    {
        CHECK(glBindTexture(GL_TEXTURE_2D, this->id));
        CHECK(glReadPixels(0, 0, this->width, this->height, this->format, this->type, outputBuffer));
    }

    void copyTo(std::size_t x, std::size_t y, std::size_t width, std::size_t height, PixelPackBuffer buffer) override
    {
        if (this->copyToFormat == GL_INVALID_ENUM)
        {
            CUBOS_ERROR("This operation is unsupported for this texture format");
            return;
        }

        if (this->framebuffer == 0)
        {
            CHECK(glGenFramebuffers(1, &this->framebuffer));
            CHECK(glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer));
            CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->id, 0));
        }
        else
        {
            CHECK(glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer));
        }

        CHECK(glViewport(0, 0, this->width, this->height));
        CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, std::static_pointer_cast<OGLPixelPackBuffer>(buffer)->id));
        CHECK(glReadPixels(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width),
                           static_cast<GLsizei>(height), this->copyToFormat, this->copyToType, nullptr));
        CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));

        CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void generateMipmaps() override
    {
        CHECK(glBindTexture(GL_TEXTURE_2D, this->id));
        CHECK(glGenerateMipmap(GL_TEXTURE_2D));
    }

    std::shared_ptr<bool> destroyed;

    GLuint id;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    GLenum copyToFormat;
    GLenum copyToType;
    GLsizei width;
    GLsizei height;
    GLuint framebuffer = 0;
};

class OGLTexture2DArray : public impl::Texture2DArray
{
public:
    OGLTexture2DArray(std::shared_ptr<bool> destroyed, GLuint id, GLenum internalFormat, GLenum format, GLenum type)
        : destroyed(std::move(destroyed))
        , id(id)
        , internalFormat(internalFormat)
        , format(format)
        , type(type)
    {
    }

    ~OGLTexture2DArray() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteTextures(1, &this->id));
        }
    }

    void update(std::size_t x, std::size_t y, std::size_t i, std::size_t width, std::size_t height, const void* data,
                std::size_t level) override
    {
        CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, this->id));
        CHECK(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(level), static_cast<GLint>(x),
                              static_cast<GLint>(y), static_cast<GLint>(i), static_cast<GLsizei>(width),
                              static_cast<GLsizei>(height), 1, this->format, this->type, data));
    }

    void generateMipmaps() override
    {
        CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, this->id));
        CHECK(glGenerateMipmap(GL_TEXTURE_2D_ARRAY));
    }

    std::shared_ptr<bool> destroyed;

    GLuint id;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};

class OGLTexture3D : public impl::Texture3D
{
public:
    OGLTexture3D(std::shared_ptr<bool> destroyed, GLuint id, GLenum internalFormat, GLenum format, GLenum type)
        : destroyed(std::move(destroyed))
        , id(id)
        , internalFormat(internalFormat)
        , format(format)
        , type(type)
    {
    }

    ~OGLTexture3D() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteTextures(1, &this->id));
        }
    }

    void update(std::size_t x, std::size_t y, std::size_t z, std::size_t width, std::size_t height, std::size_t depth,
                const void* data, std::size_t level) override
    {
        CHECK(glBindTexture(GL_TEXTURE_3D, this->id));
        CHECK(glTexSubImage3D(GL_TEXTURE_3D, static_cast<GLint>(level), static_cast<GLint>(x), static_cast<GLint>(y),
                              static_cast<GLint>(z), static_cast<GLsizei>(width), static_cast<GLsizei>(height),
                              static_cast<GLsizei>(depth), this->format, this->type, data));
    }

    void generateMipmaps() override
    {
        CHECK(glBindTexture(GL_TEXTURE_3D, this->id));
        CHECK(glGenerateMipmap(GL_TEXTURE_3D));
    }

    std::shared_ptr<bool> destroyed;

    GLuint id;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};

class OGLCubeMap : public impl::CubeMap
{
public:
    OGLCubeMap(std::shared_ptr<bool> destroyed, GLuint id, GLenum internalFormat, GLenum format, GLenum type)
        : destroyed(std::move(destroyed))
        , id(id)
        , internalFormat(internalFormat)
        , format(format)
        , type(type)
    {
    }

    ~OGLCubeMap() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteTextures(1, &this->id));
        }
    }

    void update(std::size_t x, std::size_t y, std::size_t width, std::size_t height, const void* data, CubeFace face,
                std::size_t level) override
    {
        GLenum glFace;
        cubeFaceToGL(face, glFace);

        CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, this->id));
        CHECK(glTexSubImage2D(glFace, static_cast<GLint>(level), static_cast<GLint>(x), static_cast<GLint>(y),
                              static_cast<GLsizei>(width), static_cast<GLsizei>(height), this->format, this->type,
                              data));
    }

    void generateMipmaps() override
    {
        CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, this->id));
        CHECK(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
    }

    std::shared_ptr<bool> destroyed;

    GLuint id;
    GLenum internalFormat;
    GLenum format;
    GLenum type;
};

class OGLConstantBuffer : public impl::ConstantBuffer
{
public:
    OGLConstantBuffer(std::shared_ptr<bool> destroyed, GLuint id, GLsizeiptr size)
        : destroyed(std::move(destroyed))
        , id(id)
        , size(size)
    {
    }

    ~OGLConstantBuffer() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteBuffers(1, &this->id));
        }
    }

    void fill(const void* data, std::size_t size) override
    {
        CHECK(glBindBuffer(GL_UNIFORM_BUFFER, this->id));
        CHECK(glBufferSubData(GL_UNIFORM_BUFFER, 0, static_cast<GLsizeiptr>(size), data));
    }

    std::shared_ptr<bool> destroyed;

    GLuint id;
    GLsizeiptr size;
};

class OGLIndexBuffer : public impl::IndexBuffer
{
public:
    OGLIndexBuffer(std::shared_ptr<bool> destroyed, GLuint id, GLenum format, std::size_t indexSz, GLsizeiptr size)
        : destroyed(std::move(destroyed))
        , id(id)
        , format(format)
        , indexSz(indexSz)
        , size(size)
    {
    }

    ~OGLIndexBuffer() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteBuffers(1, &this->id));
        }
    }

    void fill(const void* data, std::size_t size) override
    {
        CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id));
        CHECK(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data));
    }

    std::shared_ptr<bool> destroyed;

    GLuint id;
    GLenum format;
    std::size_t indexSz;
    GLsizeiptr size;
};

class OGLVertexBuffer : public impl::VertexBuffer
{
public:
    OGLVertexBuffer(std::shared_ptr<bool> destroyed, GLuint id, GLsizeiptr size)
        : destroyed(std::move(destroyed))
        , id(id)
        , size(size)
    {
    }

    ~OGLVertexBuffer() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteBuffers(1, &this->id));
        }
    }

    void fill(const void* data, std::size_t size) override
    {
        CHECK(glBindBuffer(GL_ARRAY_BUFFER, this->id));
        CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data));
    }

    void fill(const void* data, std::size_t offset, std::size_t size, bool synchronized = true) override
    {
#ifdef __EMSCRIPTEN__
        // On WebGL2, we can't use glMapBufferRange. Therefore, if on Emscripten, we always use glBufferSubData
        synchronized = true;
#endif
        if (synchronized)
        {
        CHECK(glBindBuffer(GL_ARRAY_BUFFER, this->id));
            CHECK(glBufferSubData(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data));
        }
        else
        {
#ifndef __EMSCRIPTEN__
            CHECK(glBindBuffer(GL_ARRAY_BUFFER, this->id));
        void* ptr;
            CHECK_VAL(ptr,
                      glMapBufferRange(GL_ARRAY_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size),
                                       GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));
            memcpy(ptr, data, size);
        CHECK(glUnmapBuffer(GL_ARRAY_BUFFER));
#endif // __EMSCRIPTEN__
        }
    }

    std::shared_ptr<bool> destroyed;

    GLuint id;
    GLsizeiptr size;
};

class OGLVertexArray : public impl::VertexArray
{
public:
    OGLVertexArray(std::shared_ptr<bool> destroyed, GLuint id, const VertexBuffer* buffers)
        : destroyed(std::move(destroyed))
        , id(id)
    {
        for (std::size_t i = 0; i < CUBOS_CORE_GL_MAX_VERTEX_ARRAY_BUFFER_COUNT; ++i)
        {
            this->buffers[i] = buffers[i];
        }
    }

    ~OGLVertexArray() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteVertexArrays(1, &this->id));
        }
    }

    std::shared_ptr<bool> destroyed;

    VertexBuffer buffers[CUBOS_CORE_GL_MAX_VERTEX_ARRAY_BUFFER_COUNT];
    GLuint id;
};

class OGLShaderStage : public impl::ShaderStage
{
public:
    OGLShaderStage(std::shared_ptr<bool> destroyed, Stage type, GLuint shader)
        : destroyed(std::move(destroyed))
        , type(type)
        , shader(shader)
    {
    }

    ~OGLShaderStage() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteShader(this->shader));
        }
    }

    Stage getType() override
    {
        return this->type;
    }

    std::shared_ptr<bool> destroyed;

    Stage type;
    GLuint shader;
};

class OGLShaderBindingPoint : public impl::ShaderBindingPoint
{
public:
    OGLShaderBindingPoint(const char* name, int loc, int tex = 0)
        : name(name)
        , loc(loc)
        , tex(tex)
    {
    }

    void bind(Sampler sampler) override
    {
        if (sampler)
        {
            CHECK(glBindSampler(static_cast<GLuint>(this->tex), std::static_pointer_cast<OGLSampler>(sampler)->id));
        }
        else
        {
            CHECK(glBindSampler(static_cast<GLuint>(this->tex), 0));
        }
    }

    void bind(Texture2D tex) override
    {
        CHECK(glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(this->tex)));
        if (tex)
        {
            CHECK(glBindTexture(GL_TEXTURE_2D, std::static_pointer_cast<OGLTexture2D>(tex)->id));
        }
        else
        {
            CHECK(glBindTexture(GL_TEXTURE_2D, 0));
        }
        CHECK(glUniform1i(this->loc, this->tex));
    }

    void bind(Texture2DArray tex) override
    {
        CHECK(glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(this->tex)));
        if (tex)
        {
            CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, std::static_pointer_cast<OGLTexture2DArray>(tex)->id));
        }
        else
        {
            CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
        }
        CHECK(glUniform1i(this->loc, this->tex));
    }

    void bind(Texture3D tex) override
    {
        CHECK(glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(this->tex)));
        if (tex)
        {
            CHECK(glBindTexture(GL_TEXTURE_3D, std::static_pointer_cast<OGLTexture3D>(tex)->id));
        }
        else
        {
            CHECK(glBindTexture(GL_TEXTURE_3D, 0));
        }
        CHECK(glUniform1i(this->loc, this->tex));
    }

    void bind(CubeMap cubeMap) override
    {
        CHECK(glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(this->tex)));
        if (cubeMap)
        {
            CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, std::static_pointer_cast<OGLCubeMap>(cubeMap)->id));
        }
        else
        {
            CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
        }
        CHECK(glUniform1i(this->loc, this->tex));
    }

    void bind(ConstantBuffer cb) override
    {
        if (cb)
        {
            CHECK(glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(this->loc),
                                   std::static_pointer_cast<OGLConstantBuffer>(cb)->id));
        }
        else
        {
            CHECK(glBindBufferBase(GL_UNIFORM_BUFFER, static_cast<GLuint>(this->loc), 0));
        }
    }

    void bind(gl::Texture2D tex, int level, Access access) override
    {
#ifndef __EMSCRIPTEN__
        auto texImpl = std::static_pointer_cast<OGLTexture2D>(tex);
        GLenum glAccess;
        switch (access)
        {
        case Access::Read:
            glAccess = GL_READ_ONLY;
            break;
        case Access::Write:
            glAccess = GL_WRITE_ONLY;
            break;
        case Access::ReadWrite:
            glAccess = GL_READ_WRITE;
            break;
        default:
            CUBOS_FAIL("Invalid enum");
        }

        CHECK(glUniform1i(this->loc, this->tex));
        CHECK(glBindImageTexture(static_cast<GLuint>(this->tex), texImpl->id, level, GL_TRUE, 0, glAccess,
                                 texImpl->internalFormat));
#else
        (void)tex;
        (void)level;
        (void)access;
        CUBOS_ERROR("Texture image binding is not supported on WebGL");
#endif
    }

    void setConstant(glm::vec2 val) override
    {
        CHECK(glUniform2fv(loc, 1, &val[0]));
    }

    void setConstant(glm::vec3 val) override
    {
        CHECK(glUniform3fv(loc, 1, &val[0]));
    }

    void setConstant(glm::vec4 val) override
    {
        CHECK(glUniform4fv(loc, 1, &val[0]));
    }

    void setConstant(glm::ivec2 val) override
    {
        CHECK(glUniform2iv(loc, 1, &val[0]));
    }

    void setConstant(glm::ivec3 val) override
    {
        CHECK(glUniform3iv(loc, 1, &val[0]));
    }

    void setConstant(glm::ivec4 val) override
    {
        CHECK(glUniform4iv(loc, 1, &val[0]));
    }

    void setConstant(glm::uvec2 val) override
    {
        CHECK(glUniform2uiv(loc, 1, &val[0]));
    }

    void setConstant(glm::uvec3 val) override
    {
        CHECK(glUniform3uiv(loc, 1, &val[0]));
    }

    void setConstant(glm::uvec4 val) override
    {
        CHECK(glUniform4uiv(loc, 1, &val[0]));
    }

    void setConstant(glm::mat4 val) override
    {
        CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, &val[0][0]));
    }

    void setConstant(float val) override
    {
        CHECK(glUniform1f(loc, val));
    }

    void setConstant(int val) override
    {
        CHECK(glUniform1i(loc, val));
    }

    void setConstant(unsigned int val) override
    {
        CHECK(glUniform1ui(loc, val));
    }

    bool queryConstantBufferStructure(ConstantBufferStructure* /*structure*/) override
    {
        return false; // TODO
    }

    std::string name;
    int loc, tex;
};

class OGLShaderPipeline : public impl::ShaderPipeline
{
public:
    OGLShaderPipeline(std::shared_ptr<bool> destroyed, ShaderStage vs, ShaderStage ps, GLuint program)
        : destroyed(std::move(destroyed))
        , vs(std::move(vs))
        , ps(std::move(ps))
        , program(program)
    {
        mTexCount = 0;
        mUboCount = 0;
        mSsboCount = 0;
    }

    OGLShaderPipeline(std::shared_ptr<bool> destroyed, ShaderStage vs, ShaderStage gs, ShaderStage ps, GLuint program)
        : OGLShaderPipeline(std::move(destroyed), std::move(vs), std::move(ps), program)
    {
        this->gs = std::move(gs);
    }

    OGLShaderPipeline(std::shared_ptr<bool> destroyed, ShaderStage cs, GLuint program)
        : destroyed(std::move(destroyed))
        , cs(std::move(cs))
        , program(program)
    {
        mTexCount = 0;
        mUboCount = 0;
        mSsboCount = 0;
    }

    ~OGLShaderPipeline() override
    {
        if (!*destroyed)
        {
            CHECK(glDeleteProgram(this->program));
        }
    }

    ShaderBindingPoint getBindingPoint(const char* name) override
    {
        // Search for already existing binding point
        for (auto& bp : this->bps)
        {
            if (bp.name == name)
            {
                return &bp;
            }
        }

        // No existing binding point was found, it must be created first

        GLint loc;
        CHECK_VAL(loc, glGetUniformLocation(this->program, name));
        if (loc != -1)
        {
            bps.emplace_back(name, loc, mTexCount++);
            return &bps.back();
        }

        // Search for uniform block binding
        GLuint index;
        CHECK_VAL(index, glGetUniformBlockIndex(this->program, name));
        if (index != GL_INVALID_INDEX)
        {
            auto loc = mUboCount;
            CHECK(glUniformBlockBinding(this->program, index, static_cast<GLuint>(loc)));
            CLEANUP_GL_ERROR_RET(nullptr, (void)nullptr);

            mUboCount += 1;
            bps.emplace_back(name, loc);
            return &bps.back();
        }

#ifndef __EMSCRIPTEN__
        // Search for shader storage block binding
        index = glGetProgramResourceIndex(this->program, GL_SHADER_STORAGE_BLOCK, name);
        CHECK_GL_ERROR(glGetProgramResourceIndex);
        if (index != GL_INVALID_INDEX)
        {
            auto loc = mSsboCount;
            CHECK(glShaderStorageBlockBinding(this->program, index, static_cast<GLuint>(loc)));
            CLEANUP_GL_ERROR_RET(nullptr, (void)nullptr);

            mSsboCount += 1;
            bps.emplace_back(name, loc);
            return &bps.back();
        }
#endif

        return nullptr;
    }

    std::shared_ptr<bool> destroyed;

    ShaderStage vs, gs, ps, cs;
    GLuint program;
    std::list<OGLShaderBindingPoint> bps;

private:
    int mTexCount;
    int mUboCount;
    int mSsboCount;
};

#ifndef __EMSCRIPTEN__
class OGLTimer : public impl::Timer
{
public:
    OGLTimer(std::shared_ptr<bool> destroyed, GLuint beginId, GLuint endId)
        : destroyed(std::move(destroyed))
    {
        ids[0] = beginId;
        ids[1] = endId;
    }

    ~OGLTimer() override
    {
        if (!*destroyed)
        {
            glDeleteQueries(2, this->ids);
        }
    }

    void begin() override
    {
        glQueryCounter(ids[0], GL_TIMESTAMP);
        GLenum glErr = glGetError();
        if (glErr != 0)
        {
            LOG_GL_ERROR(glErr);
        }
    }

    void end() override
    {
        glQueryCounter(ids[1], GL_TIMESTAMP);
        GLenum glErr = glGetError();
        if (glErr != 0)
        {
            LOG_GL_ERROR(glErr);
        }
    }

    bool done() override
    {
        int available;
        // If "end" query is done, then "begin" query is done too, so only check "end"
        glGetQueryObjectiv(ids[1], GL_QUERY_RESULT_AVAILABLE, &available);
        GLenum glErr = glGetError();
        if (glErr != 0)
        {
            LOG_GL_ERROR(glErr);
            return false;
        }
        return static_cast<bool>(available);
    }

    int result() override
    {
        int beginTime;
        int endTime;
        glGetQueryObjectiv(ids[0], GL_QUERY_RESULT, &beginTime);
        glGetQueryObjectiv(ids[1], GL_QUERY_RESULT, &endTime);
        GLenum glErr = glGetError();
        if (glErr != 0)
        {
            LOG_GL_ERROR(glErr);
            return -1;
        }
        return endTime - beginTime;
    }

    std::shared_ptr<bool> destroyed;

    GLuint ids[2]; // begin, end
};
#endif // __EMSCRIPTEN__

OGLRenderDevice::~OGLRenderDevice()
{
    *mDestroyed = true;
}

OGLRenderDevice::OGLRenderDevice()
{
#ifndef __EMSCRIPTEN__
    // Set the debug message callback
    // TODO: disable this on release for performance reasons (?)
    if (this->OGLRenderDevice::getProperty(Property::ComputeSupported) != 0)
    {
        CHECK(glEnable(GL_DEBUG_OUTPUT));
        CHECK(glDebugMessageCallback(messageCallback, nullptr));
    }
#else
    // Check if GL_EXT_color_buffer_float is supported
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    if (strstr(extensions, "GL_EXT_color_buffer_float") == nullptr)
    {
        CUBOS_WARN("GL_EXT_color_buffer_float is required to use floating point textures as render targets");
    }
#endif

    // Create default states
    mDefaultRS = OGLRenderDevice::createRasterState({});
    mDefaultDSS = OGLRenderDevice::createDepthStencilState({});
    mDefaultBS = OGLRenderDevice::createBlendState({});
}

Framebuffer OGLRenderDevice::createFramebuffer(const FramebufferDesc& desc)
{
    // Validate arguments
    if (desc.targetCount > CUBOS_CORE_GL_MAX_FRAMEBUFFER_RENDER_TARGET_COUNT)
    {
        CUBOS_ERROR("Framebuffer can only have at most {} render targets",
                    CUBOS_CORE_GL_MAX_FRAMEBUFFER_RENDER_TARGET_COUNT);
        return nullptr;
    }

    for (uint32_t i = 0; i < desc.targetCount; ++i)
    {
        switch (desc.targets[i].getTargetType())
        {
        case FramebufferDesc::TargetType::CubeMap:
            if (desc.targets[i].getCubeMapTarget().handle == nullptr)
            {
                CUBOS_ERROR("Target {} is nullptr", i);
                return nullptr;
            }
            break;
        case FramebufferDesc::TargetType::Texture2D:
            if (desc.targets[i].getTexture2DTarget().handle == nullptr)
            {
                CUBOS_ERROR("Target {} is nullptr", i);
                return nullptr;
            }
            break;
        }
    }

    // Initialize framebuffer
    GLuint id;
    CHECK(glGenFramebuffers(1, &id));
    CHECK(glBindFramebuffer(GL_FRAMEBUFFER, id));

    // Attach targets
    std::vector<GLenum> drawBuffers;
    for (uint32_t i = 0; i < desc.targetCount; ++i)
    {
        drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);

        switch (desc.targets[i].getTargetType())
        {
        case FramebufferDesc::TargetType::CubeMap:
            GLenum face;
            cubeFaceToGL(desc.targets[i].getCubeMapTarget().face, face);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, face,
                                   std::static_pointer_cast<OGLCubeMap>(desc.targets[i].getCubeMapTarget().handle)->id,
                                   static_cast<GLint>(desc.targets[i].mipLevel));
            break;
        case FramebufferDesc::TargetType::Texture2D:
            glFramebufferTexture2D(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                std::static_pointer_cast<OGLTexture2D>(desc.targets[i].getTexture2DTarget().handle)->id,
                static_cast<GLint>(desc.targets[i].mipLevel));
            break;
        }
    }

    // Attach depth stencil texture
    if (desc.depthStencil.isSet())
    {
        bool formatError = false;
        switch (desc.depthStencil.getTargetType())
        {
        case FramebufferDesc::TargetType::CubeMap: {
            const auto& target = desc.depthStencil.getCubeMapTarget();
            GLenum face;
            cubeFaceToGL(target.face, face);
            auto ds = std::static_pointer_cast<OGLCubeMap>(target.handle);
            if (ds->format == GL_DEPTH_COMPONENT)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, face, ds->id,
                                       static_cast<GLint>(desc.depthStencil.mipLevel));
            }
            else if (ds->format == GL_DEPTH_STENCIL)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, ds->id,
                                       static_cast<GLint>(desc.depthStencil.mipLevel));
            }
            else
            {
                formatError = true;
            }
            break;
        }
        case FramebufferDesc::TargetType::Texture2D: {
            auto ds = std::static_pointer_cast<OGLTexture2D>(desc.depthStencil.getTexture2DTarget().handle);
            if (ds->format == GL_DEPTH_COMPONENT)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ds->id,
                                       static_cast<GLint>(desc.depthStencil.mipLevel));
            }
            else if (ds->format == GL_DEPTH_STENCIL)
            {
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, ds->id,
                                       static_cast<GLint>(desc.depthStencil.mipLevel));
            }
            else
            {
                formatError = true;
            }
            break;
        }
        }

        if (formatError)
        {
            CHECK(glDeleteFramebuffers(1, &id));
            CUBOS_ERROR("Invalid depth stencil target format");
            return nullptr;
        }
    }

    // Define draw buffers
    if (drawBuffers.empty())
    {
        CHECK(glDrawBuffer(GL_NONE));
        CHECK(glReadBuffer(GL_NONE));
    }
    else
    {
        CHECK(glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data()));
    }

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteFramebuffers(1, &id));

    // Check if the framebuffer is complete
    GLenum status;
    CHECK_VAL(status, glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        CHECK(glDeleteFramebuffers(1, &id));
        CUBOS_ERROR("glCheckFramebufferStatus didn't return GL_FRAMEBUFFER_COMPLETE (returned {})", status);
        return nullptr;
    }

    return std::make_shared<OGLFramebuffer>(mDestroyed, id);
}

void OGLRenderDevice::setFramebuffer(Framebuffer fb)
{
    if (fb)
    {
        CHECK(glBindFramebuffer(GL_FRAMEBUFFER, std::static_pointer_cast<OGLFramebuffer>(fb)->id));
    }
    else
    {
        CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}

RasterState OGLRenderDevice::createRasterState(const RasterStateDesc& desc)
{
    auto rs = std::make_shared<OGLRasterState>();
    rs->cullEnabled = static_cast<GLboolean>(desc.cullEnabled);
    rs->scissorEnabled = static_cast<GLboolean>(desc.scissorEnabled);
    faceToGL(desc.cullFace, rs->cullFace);
    windingToGL(desc.frontFace, rs->frontFace);
    return rs;
}

void OGLRenderDevice::setRasterState(RasterState rs)
{
    auto rsImpl = std::static_pointer_cast<OGLRasterState>(rs ? rs : mDefaultRS);

    if (rsImpl->cullEnabled == 0U)
    {
        CHECK(glDisable(GL_CULL_FACE));
    }
    else
    {
        CHECK(glEnable(GL_CULL_FACE));
        CHECK(glCullFace(rsImpl->cullFace));
    }

    if (rsImpl->scissorEnabled == 0U)
    {
        CHECK(glDisable(GL_SCISSOR_TEST));
    }
    else
    {
        CHECK(glEnable(GL_SCISSOR_TEST));
    }

    CHECK(glFrontFace(rsImpl->frontFace));
}

DepthStencilState OGLRenderDevice::createDepthStencilState(const DepthStencilStateDesc& desc)
{
    auto dss = std::make_shared<OGLDepthStencilState>();

    dss->depthEnabled = static_cast<GLboolean>(desc.depth.enabled);
    dss->depthWriteEnabled = static_cast<GLboolean>(desc.depth.writeEnabled);
    dss->depthNear = desc.depth.near;
    dss->depthFar = desc.depth.far;
    compareToGL(desc.depth.compare, dss->depthFunc);

    dss->stencilEnabled = static_cast<GLboolean>(desc.stencil.enabled);
    dss->stencilRef = desc.stencil.ref;
    dss->stencilReadMask = desc.stencil.readMask;
    dss->stencilWriteMask = desc.stencil.writeMask;

    compareToGL(desc.stencil.frontFace.compare, dss->frontStencilFunc);
    stencilActionToGL(desc.stencil.frontFace.fail, dss->frontFaceStencilFail);
    stencilActionToGL(desc.stencil.frontFace.pass, dss->frontFaceStencilPass);
    stencilActionToGL(desc.stencil.frontFace.depthFail, dss->frontFaceDepthFail);

    compareToGL(desc.stencil.backFace.compare, dss->backStencilFunc);
    stencilActionToGL(desc.stencil.backFace.fail, dss->backFaceStencilFail);
    stencilActionToGL(desc.stencil.backFace.pass, dss->backFaceStencilPass);
    stencilActionToGL(desc.stencil.backFace.depthFail, dss->backFaceDepthFail);

    return dss;
}

void OGLRenderDevice::setDepthStencilState(DepthStencilState dss)
{
    auto dssImpl = std::static_pointer_cast<OGLDepthStencilState>(dss ? dss : mDefaultDSS);

    if (dssImpl->depthEnabled == 0U)
    {
        CHECK(glDisable(GL_DEPTH_TEST));
    }
    else
    {
        CHECK(glEnable(GL_DEPTH_TEST));
        CHECK(glDepthFunc(dssImpl->depthFunc));
        CHECK(glDepthMask(dssImpl->depthWriteEnabled));
#ifndef __EMSCRIPTEN__
        CHECK(glDepthRange(dssImpl->depthNear, dssImpl->depthFar));
#else
        CHECK(glDepthRangef(dssImpl->depthNear, dssImpl->depthFar));
#endif
    }

    if (dssImpl->stencilEnabled == 0U)
    {
        CHECK(glDisable(GL_STENCIL_TEST));
    }
    else
    {
        CHECK(glEnable(GL_STENCIL_TEST));

        CHECK(glStencilFuncSeparate(GL_FRONT, dssImpl->frontStencilFunc, static_cast<GLint>(dssImpl->stencilRef),
                                    dssImpl->stencilReadMask));
        CHECK(glStencilMaskSeparate(GL_FRONT, dssImpl->stencilWriteMask));
        CHECK(glStencilOpSeparate(GL_FRONT, dssImpl->frontFaceStencilFail, dssImpl->frontFaceDepthFail,
                                  dssImpl->frontFaceStencilPass));
        CHECK(glStencilFuncSeparate(GL_BACK, dssImpl->backStencilFunc, static_cast<GLint>(dssImpl->stencilRef),
                                    dssImpl->stencilReadMask));
        CHECK(glStencilMaskSeparate(GL_BACK, dssImpl->stencilWriteMask));
        CHECK(glStencilOpSeparate(GL_BACK, dssImpl->backFaceStencilFail, dssImpl->backFaceDepthFail,
                                  dssImpl->backFaceStencilPass));
    }
}

BlendState OGLRenderDevice::createBlendState(const BlendStateDesc& desc)
{
    auto bs = std::make_shared<OGLBlendState>();

    bs->blendEnabled = static_cast<GLboolean>(desc.blendEnabled);
    blendFactorToGL(desc.alpha.src, bs->srcAlphaFactor);
    blendFactorToGL(desc.alpha.dst, bs->dstAlphaFactor);
    blendOpToGL(desc.alpha.op, bs->alphaBlendOp);
    blendFactorToGL(desc.color.src, bs->srcFactor);
    blendFactorToGL(desc.color.dst, bs->dstFactor);
    blendOpToGL(desc.color.op, bs->blendOp);

    return bs;
}

void OGLRenderDevice::setBlendState(BlendState bs)
{
    auto bsImpl = std::static_pointer_cast<OGLBlendState>(bs ? bs : mDefaultBS);

    if (bsImpl->blendEnabled == 0U)
    {
        CHECK(glDisable(GL_BLEND));
    }
    else
    {
        CHECK(glEnable(GL_BLEND));
        CHECK(
            glBlendFuncSeparate(bsImpl->srcFactor, bsImpl->dstFactor, bsImpl->srcAlphaFactor, bsImpl->dstAlphaFactor));
        CHECK(glBlendEquationSeparate(bsImpl->blendOp, bsImpl->alphaBlendOp));
    }
}

Sampler OGLRenderDevice::createSampler(const SamplerDesc& desc)
{
    GLenum addressU;
    GLenum addressV;
    GLenum addressW;

    addressToGL(desc.addressU, addressU);
    addressToGL(desc.addressV, addressV);
    addressToGL(desc.addressW, addressW);

    GLenum minFilter;
    GLenum magFilter;

    switch (desc.minFilter)
    {
    case TextureFilter::Nearest:
        if (desc.mipmapFilter == TextureFilter::None)
        {
            minFilter = GL_NEAREST;
        }
        else if (desc.mipmapFilter == TextureFilter::Nearest)
        {
            minFilter = GL_NEAREST_MIPMAP_NEAREST;
        }
        else if (desc.mipmapFilter == TextureFilter::Linear)
        {
            minFilter = GL_NEAREST_MIPMAP_LINEAR;
        }
        else
        {
            abort(); // Invalid enum value
        }
        break;
    case TextureFilter::Linear:
        if (desc.mipmapFilter == TextureFilter::None)
        {
            minFilter = GL_LINEAR;
        }
        else if (desc.mipmapFilter == TextureFilter::Nearest)
        {
            minFilter = GL_LINEAR_MIPMAP_NEAREST;
        }
        else if (desc.mipmapFilter == TextureFilter::Linear)
        {
            minFilter = GL_LINEAR_MIPMAP_LINEAR;
        }
        else
        {
            abort(); // Invalid enum value
        }
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
    CHECK(glGenSamplers(1, &id));
    CHECK(glSamplerParameteri(id, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minFilter)));
    CHECK(glSamplerParameteri(id, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(magFilter)));
    if (GL_EXT_texture_filter_anisotropic != 0)
    {
        CHECK(glSamplerParameteri(id, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(desc.maxAnisotropy)));
    }
    CHECK(glSamplerParameteri(id, GL_TEXTURE_WRAP_S, static_cast<GLint>(addressU)));
    CHECK(glSamplerParameteri(id, GL_TEXTURE_WRAP_T, static_cast<GLint>(addressV)));
    CHECK(glSamplerParameteri(id, GL_TEXTURE_WRAP_R, static_cast<GLint>(addressW)));

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteSamplers(1, &id));

    return std::make_shared<OGLSampler>(mDestroyed, id);
}

Texture2D OGLRenderDevice::createTexture2D(const Texture2DDesc& desc)
{
    GLenum internalFormat;
    GLenum format;
    GLenum type;
    GLenum copyToFormat = GL_INVALID_ENUM;
    GLenum copyToType = GL_INVALID_ENUM;

    if (!textureFormatToGL(desc.format, internalFormat, format, type))
    {
        CUBOS_ERROR("Unsupported texture format {}", static_cast<int>(desc.format));
        return nullptr;
    }

    textureFormatToGLForCopying(desc.format, copyToFormat, copyToType);

    // Initialize texture
    GLuint id;
    CHECK(glGenTextures(1, &id));
    CHECK(glBindTexture(GL_TEXTURE_2D, id));
    CHECK(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
    for (std::size_t i = 0, div = 1; i < desc.mipLevelCount; ++i, div *= 2)
    {
        CHECK(glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(i), static_cast<GLint>(internalFormat),
                           static_cast<GLsizei>(desc.width / div), static_cast<GLsizei>(desc.height / div), 0, format,
                           type, desc.data[i]));
    }
    CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    if (GL_EXT_texture_filter_anisotropic != 0)
    {
        CHECK(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0F));
    }

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteTextures(1, &id));

    return std::make_shared<OGLTexture2D>(mDestroyed, id, internalFormat, format, type, copyToFormat, copyToType,
                                          static_cast<GLsizei>(desc.width), static_cast<GLsizei>(desc.height));
}

Texture2DArray OGLRenderDevice::createTexture2DArray(const Texture2DArrayDesc& desc)
{
    GLenum internalFormat;
    GLenum format;
    GLenum type;

    if (!textureFormatToGL(desc.format, internalFormat, format, type))
    {
        CUBOS_ERROR("Unsupported texture format {}", static_cast<int>(desc.format));
        return nullptr;
    }

    // Initialize texture
    GLuint id;
    CHECK(glGenTextures(1, &id));
    CHECK(glBindTexture(GL_TEXTURE_2D_ARRAY, id));
    for (std::size_t j = 0; j < desc.mipLevelCount; ++j)
    {
        CHECK(glTexImage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(j), static_cast<GLint>(internalFormat),
                           static_cast<GLsizei>(desc.width), static_cast<GLsizei>(desc.height),
                           static_cast<GLsizei>(desc.size), 0, format, type, nullptr));
    }
    for (std::size_t i = 0; i < desc.size; ++i)
    {
        for (std::size_t j = 0, div = 1; j < desc.mipLevelCount; ++j, div *= 2)
        {
            if (desc.data[i][j] != nullptr)
            {
                CHECK(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, static_cast<GLint>(j), 0, 0, static_cast<GLint>(i),
                                      static_cast<GLsizei>(desc.width / div), static_cast<GLsizei>(desc.height / div),
                                      1, format, type, desc.data[i][j]));
            }
        }
    }
    CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CHECK(glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    if (GL_EXT_texture_filter_anisotropic != 0)
    {
        CHECK(glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0F));
    }

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteTextures(1, &id));

    return std::make_shared<OGLTexture2DArray>(mDestroyed, id, internalFormat, format, type);
}

Texture3D OGLRenderDevice::createTexture3D(const Texture3DDesc& desc)
{
    if (desc.format == TextureFormat::Depth16 || desc.format == TextureFormat::Depth32 ||
        desc.format == TextureFormat::Depth24Stencil8 || desc.format == TextureFormat::Depth32Stencil8)
    {
        CUBOS_ERROR("Depth/stencil formats are not supported on 3D textures");
        return nullptr;
    }

    GLenum internalFormat;
    GLenum format;
    GLenum type;

    if (!textureFormatToGL(desc.format, internalFormat, format, type))
    {
        CUBOS_ERROR("Unsupported texture format {}", static_cast<int>(desc.format));
        return nullptr;
    }

    // Initialize texture
    GLuint id;
    CHECK(glGenTextures(1, &id));
    CHECK(glBindTexture(GL_TEXTURE_3D, id));
    for (std::size_t i = 0, div = 1; i < desc.mipLevelCount; ++i, div *= 2)
    {
        CHECK(glTexImage3D(GL_TEXTURE_3D, static_cast<GLint>(i), static_cast<GLint>(internalFormat),
                           static_cast<GLsizei>(desc.width / div), static_cast<GLsizei>(desc.height / div),
                           static_cast<GLsizei>(desc.depth / div), 0, format, type, desc.data[i]));
    }
    CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
    if (GL_EXT_texture_filter_anisotropic != 0)
    {
        CHECK(glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0F));
    }

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteTextures(1, &id));

    return std::make_shared<OGLTexture3D>(mDestroyed, id, internalFormat, format, type);
}

CubeMap OGLRenderDevice::createCubeMap(const CubeMapDesc& desc)
{
    GLenum internalFormat;
    GLenum format;
    GLenum type;

    if (!textureFormatToGL(desc.format, internalFormat, format, type))
    {
        CUBOS_ERROR("Unsupported texture format {}", static_cast<int>(desc.format));
        return nullptr;
    }

    // Initialize texture
    GLuint id;
    CHECK(glGenTextures(1, &id));
    CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
    for (std::size_t i = 0, div = 1; i < desc.mipLevelCount; ++i, div *= 2)
    {
        CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, static_cast<GLint>(i), static_cast<GLint>(internalFormat),
                           static_cast<GLsizei>(desc.width / div), static_cast<GLsizei>(desc.height / div), 0, format,
                           type, desc.data[static_cast<int>(CubeFace::PositiveX)][i]));
        CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, static_cast<GLint>(i), static_cast<GLint>(internalFormat),
                           static_cast<GLsizei>(desc.width / div), static_cast<GLsizei>(desc.height / div), 0, format,
                           type, desc.data[static_cast<int>(CubeFace::NegativeX)][i]));
        CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, static_cast<GLint>(i), static_cast<GLint>(internalFormat),
                           static_cast<GLsizei>(desc.width / div), static_cast<GLsizei>(desc.height / div), 0, format,
                           type, desc.data[static_cast<int>(CubeFace::PositiveY)][i]));
        CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, static_cast<GLint>(i), static_cast<GLint>(internalFormat),
                           static_cast<GLsizei>(desc.width / div), static_cast<GLsizei>(desc.height / div), 0, format,
                           type, desc.data[static_cast<int>(CubeFace::NegativeY)][i]));
        CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, static_cast<GLint>(i), static_cast<GLint>(internalFormat),
                           static_cast<GLsizei>(desc.width / div), static_cast<GLsizei>(desc.height / div), 0, format,
                           type, desc.data[static_cast<int>(CubeFace::PositiveZ)][i]));
        CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, static_cast<GLint>(i), static_cast<GLint>(internalFormat),
                           static_cast<GLsizei>(desc.width / div), static_cast<GLsizei>(desc.height / div), 0, format,
                           type, desc.data[static_cast<int>(CubeFace::NegativeZ)][i]));
    }
    CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    if (GL_EXT_texture_filter_anisotropic != 0)
    {
        CHECK(glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0F));
    }

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteTextures(1, &id));

    return std::make_shared<OGLCubeMap>(mDestroyed, id, internalFormat, format, type);
}

PixelPackBuffer OGLRenderDevice::createPixelPackBuffer(std::size_t size)
{
    // Initialize buffer
    GLuint id;
    CHECK(glGenBuffers(1, &id));
    CHECK(glBindBuffer(GL_PIXEL_PACK_BUFFER, id));
    CHECK(glBufferData(GL_PIXEL_PACK_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_STREAM_COPY));

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteBuffers(1, &id));

    return std::make_shared<OGLPixelPackBuffer>(mDestroyed, id, static_cast<GLsizeiptr>(size));
}

ConstantBuffer OGLRenderDevice::createConstantBuffer(std::size_t size, const void* data, Usage usage)
{
    // Validate arguments
    if (usage == Usage::Static && data == nullptr)
    {
        abort();
    }

    GLenum glUsage;
    if (usage == Usage::Default || usage == Usage::Static)
    {
        glUsage = GL_STATIC_DRAW;
    }
    else if (usage == Usage::Dynamic)
    {
        glUsage = GL_DYNAMIC_DRAW;
    }
    else
    {
        abort(); // Invalid enum value
    }

    // Initialize buffer
    GLuint id;
    CHECK(glGenBuffers(1, &id));
    CHECK(glBindBuffer(GL_UNIFORM_BUFFER, id));
    CHECK(glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(size), data, glUsage));

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteBuffers(1, &id));

    return std::make_shared<OGLConstantBuffer>(mDestroyed, id, static_cast<GLsizeiptr>(size));
}

IndexBuffer OGLRenderDevice::createIndexBuffer(std::size_t size, const void* data, IndexFormat format, Usage usage)
{
    // Validate arguments
    if (usage == Usage::Static && data == nullptr)
    {
        abort();
    }

    GLenum glUsage;
    if (usage == Usage::Default || usage == Usage::Static)
    {
        glUsage = GL_STATIC_DRAW;
    }
    else if (usage == Usage::Dynamic)
    {
        glUsage = GL_DYNAMIC_DRAW;
    }
    else
    {
        abort(); // Invalid enum value
    }

    GLenum glFormat;
    std::size_t indexSz;
    if (format == IndexFormat::UShort)
    {
        glFormat = GL_UNSIGNED_SHORT;
        indexSz = 2;
    }
    else if (format == IndexFormat::UInt)
    {
        glFormat = GL_UNSIGNED_INT;
        indexSz = 4;
    }
    else
    {
        abort(); // Invalid enum value
    }

    // Initialize buffer
    GLuint id;
    CHECK(glGenBuffers(1, &id));
    CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
    CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, glUsage));

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteBuffers(1, &id));

    return std::make_shared<OGLIndexBuffer>(mDestroyed, id, glFormat, indexSz, static_cast<GLsizeiptr>(size));
}

void OGLRenderDevice::setIndexBuffer(IndexBuffer ib)
{
    auto ibImpl = std::static_pointer_cast<OGLIndexBuffer>(ib);
    CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibImpl->id));
    mCurrentIndexFormat = static_cast<int>(ibImpl->format);
    mCurrentIndexSz = ibImpl->indexSz;
}

VertexBuffer OGLRenderDevice::createVertexBuffer(std::size_t size, const void* data, Usage usage)
{
    // Validate arguments
    if (usage == Usage::Static && data == nullptr)
    {
        abort();
    }

    GLenum glUsage;
    if (usage == Usage::Default || usage == Usage::Static)
    {
        glUsage = GL_STATIC_DRAW;
    }
    else if (usage == Usage::Dynamic)
    {
        glUsage = GL_DYNAMIC_DRAW;
    }
    else
    {
        abort(); // Invalid enum value
    }

    // Initialize buffer
    GLuint id;
    CHECK(glGenBuffers(1, &id));
    CHECK(glBindBuffer(GL_ARRAY_BUFFER, id));
    CHECK(glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, glUsage));

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteBuffers(1, &id));

    return std::make_shared<OGLVertexBuffer>(mDestroyed, id, static_cast<GLsizeiptr>(size));
}

VertexArray OGLRenderDevice::createVertexArray(const VertexArrayDesc& desc)
{
    auto pp = std::static_pointer_cast<OGLShaderPipeline>(desc.shaderPipeline);
    assert(pp != nullptr);

    // Initialize vertex array
    GLuint id;
    CHECK(glGenVertexArrays(1, &id));
    CHECK(glBindVertexArray(id));

    // Link elements
    assert(desc.elementCount <= CUBOS_CORE_GL_MAX_VERTEX_ARRAY_ELEMENT_COUNT);
    for (std::size_t i = 0; i < desc.elementCount; ++i)
    {
        // Get buffer
        auto vb = std::static_pointer_cast<OGLVertexBuffer>(desc.buffers[desc.elements[i].buffer.index]);
        assert(vb != nullptr);
        CHECK(glBindBuffer(GL_ARRAY_BUFFER, vb->id));

        // Get attribute location
        GLint loc;
        CHECK_VAL(loc, glGetAttribLocation(pp->program, desc.elements[i].name));
        if (loc == -1)
        {
            CHECK(glDeleteVertexArrays(1, &id));
            CUBOS_ERROR("Could not find vertex element with name {}", desc.elements[i].name);
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

        CHECK(glEnableVertexAttribArray(static_cast<GLuint>(loc)));
        if (!integer)
        {
            CHECK(glVertexAttribPointer(static_cast<GLuint>(loc), (GLint)desc.elements[i].size, type,
                                        static_cast<GLboolean>(normalized), (GLsizei)desc.elements[i].buffer.stride,
                                        (const void*)desc.elements[i].buffer.offset));
        }
        else
        {
            CHECK(glVertexAttribIPointer(static_cast<GLuint>(loc), (GLint)desc.elements[i].size, type,
                                         (GLsizei)desc.elements[i].buffer.stride,
                                         (const void*)desc.elements[i].buffer.offset));
        }
    }

    // Check errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteVertexArrays(1, &id));

    return std::make_shared<OGLVertexArray>(mDestroyed, id, desc.buffers);
}

void OGLRenderDevice::setVertexArray(VertexArray va)
{
    CHECK(glBindVertexArray(std::static_pointer_cast<OGLVertexArray>(va)->id));
}

ShaderStage OGLRenderDevice::createShaderStage(Stage stage, const char* src)
{
    GLenum shaderType;
    switch (stage)
    {
    case Stage::Vertex:
        shaderType = GL_VERTEX_SHADER;
        break;
    case Stage::Pixel:
        shaderType = GL_FRAGMENT_SHADER;
        break;
#ifndef __EMSCRIPTEN__
    case Stage::Geometry:
        shaderType = GL_GEOMETRY_SHADER;
        break;
    case Stage::Compute:
        shaderType = GL_COMPUTE_SHADER;
        break;
#endif
    default:
        CUBOS_CRITICAL("Unsupported shader stage type");
        return nullptr;
    }

#ifdef __EMSCRIPTEN__
#define GLSL_HEADER                                                                                                    \
    "#version 300 es\n"                                                                                                \
    "#define ES\n"                                                                                                     \
    "precision highp float;\n"
#else
#define GLSL_HEADER "#version 330 core\n"
#endif

    // Initialize shader
    GLuint id;
    CHECK_VAL(id, glCreateShader(shaderType));
    const char* srcs[] = {GLSL_HEADER, src};
    CHECK(glShaderSource(id, 2, (const GLchar* const*)srcs, nullptr));
    CHECK(glCompileShader(id));

    // Check for errors
    GLint success;
    CHECK(glGetShaderiv(id, GL_COMPILE_STATUS, &success));
    if (success == 0)
    {
        GLchar infoLog[512];
        CHECK(glGetShaderInfoLog(id, sizeof(infoLog), nullptr, infoLog));
        CHECK(glDeleteShader(id));
        CUBOS_ERROR("Could not compile shader: {}", infoLog);
        return nullptr;
    }

    // Check for OpenGL errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteShader(id));

    return std::make_shared<OGLShaderStage>(mDestroyed, stage, id);
}

ShaderPipeline OGLRenderDevice::createShaderPipeline(ShaderStage vs, ShaderStage ps)
{
    auto vsImpl = std::static_pointer_cast<OGLShaderStage>(vs);
    auto psImpl = std::static_pointer_cast<OGLShaderStage>(ps);

    // Initialize program
    GLuint id;
    CHECK_VAL(id, glCreateProgram());
    CHECK(glAttachShader(id, vsImpl->shader));
    CHECK(glAttachShader(id, psImpl->shader));
    CHECK(glLinkProgram(id));

    // Check for linking errors
    GLint success;
    CHECK(glGetProgramiv(id, GL_LINK_STATUS, &success));
    if (success == 0)
    {
        char infoLog[512];
        CHECK(glGetProgramInfoLog(id, sizeof(infoLog), nullptr, infoLog));
        CHECK(glDeleteProgram(id));
        CUBOS_ERROR("Could not link program (shader pipeline): {}", infoLog);
        return nullptr;
    }

    // Check for OpenGL errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteProgram(id));

    return std::make_shared<OGLShaderPipeline>(mDestroyed, vsImpl, psImpl, id);
}

ShaderPipeline OGLRenderDevice::createShaderPipeline(ShaderStage vs, ShaderStage gs, ShaderStage ps)
{
    auto vsImpl = std::static_pointer_cast<OGLShaderStage>(vs);
    auto gsImpl = std::static_pointer_cast<OGLShaderStage>(gs);
    auto psImpl = std::static_pointer_cast<OGLShaderStage>(ps);

    // Initialize program
    GLuint id;
    CHECK_VAL(id, glCreateProgram());
    CHECK(glAttachShader(id, vsImpl->shader));
    CHECK(glAttachShader(id, gsImpl->shader));
    CHECK(glAttachShader(id, psImpl->shader));
    CHECK(glLinkProgram(id));

    // Check for linking errors
    GLint success;
    CHECK(glGetProgramiv(id, GL_LINK_STATUS, &success));
    if (success == 0)
    {
        char infoLog[512];
        CHECK(glGetProgramInfoLog(id, sizeof(infoLog), nullptr, infoLog));
        CHECK(glDeleteProgram(id));
        CUBOS_ERROR("Could not link program (shader pipeline): {}", infoLog);
        return nullptr;
    }

    // Check for OpenGL errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteProgram(id));

    return std::make_shared<OGLShaderPipeline>(mDestroyed, vsImpl, gsImpl, psImpl, id);
}

ShaderPipeline OGLRenderDevice::createShaderPipeline(ShaderStage cs)
{
    auto csImpl = std::static_pointer_cast<OGLShaderStage>(cs);

    // Initialize program
    GLuint id;
    CHECK_VAL(id, glCreateProgram());
    CHECK(glAttachShader(id, csImpl->shader));
    CHECK(glLinkProgram(id));

    // Check for linking errors
    GLint success;
    CHECK(glGetProgramiv(id, GL_LINK_STATUS, &success));
    if (success == 0)
    {
        char infoLog[512];
        CHECK(glGetProgramInfoLog(id, sizeof(infoLog), nullptr, infoLog));
        CHECK(glDeleteProgram(id));
        CUBOS_ERROR("Could not link program (shader pipeline): {}", infoLog);
        return nullptr;
    }

    // Check for OpenGL errors
    CLEANUP_GL_ERROR_RET(nullptr, glDeleteProgram(id));

    return std::make_shared<OGLShaderPipeline>(mDestroyed, csImpl, id);
}

void OGLRenderDevice::setShaderPipeline(ShaderPipeline pipeline)
{
    CHECK(glUseProgram(std::static_pointer_cast<OGLShaderPipeline>(pipeline)->program));
}

Timer OGLRenderDevice::createTimer()
{
#ifdef __EMSCRIPTEN__
    CUBOS_ERROR("Timers are not supported on Emscripten");
    return nullptr;
#else
    // Initialize query object
    GLuint ids[2];
    glGenQueries(2, ids);

    // Check errors
    GLenum glErr = glGetError();
    if (glErr != 0)
    {
        glDeleteQueries(2, ids);
        LOG_GL_ERROR(glErr);
        return nullptr;
    }

    return std::make_shared<OGLTimer>(mDestroyed, ids[0], ids[1]);
#endif // __EMSCRIPTEN__
}

void OGLRenderDevice::clearColor(float r, float g, float b, float a)
{
    CHECK(glClearColor(r, g, b, a));
    CHECK(glClear(GL_COLOR_BUFFER_BIT));
}

void OGLRenderDevice::clearTargetColor(std::size_t target, float r, float g, float b, float a)
{
    float color[] = {r, g, b, a};
    CHECK(glClearBufferfv(GL_COLOR, static_cast<GLint>(target), color));
}

void OGLRenderDevice::clearTargetColor(std::size_t target, int r, int g, int b, int a)
{
    int color[] = {r, g, b, a};
    CHECK(glClearBufferiv(GL_COLOR, static_cast<GLint>(target), color));
}

void OGLRenderDevice::clearTargetColor(std::size_t target, unsigned int r, unsigned int g, unsigned int b,
                                       unsigned int a)
{
    unsigned int color[] = {r, g, b, a};
    CHECK(glClearBufferuiv(GL_COLOR, static_cast<GLint>(target), color));
}

void OGLRenderDevice::clearDepth(float depth)
{
#ifndef __EMSCRIPTEN__
    CHECK(glClearDepth(depth));
#else
    CHECK(glClearDepthf(depth));
#endif
    CHECK(glClear(GL_DEPTH_BUFFER_BIT));
}

void OGLRenderDevice::clearStencil(int stencil)
{
    CHECK(glClearStencil(stencil));
    CHECK(glClear(GL_STENCIL_BUFFER_BIT));
}

void OGLRenderDevice::drawTriangles(std::size_t offset, std::size_t count)
{
    CHECK(glDrawArrays(GL_TRIANGLES, static_cast<GLint>(offset), static_cast<GLsizei>(count)));
}

void OGLRenderDevice::drawLines(std::size_t offset, std::size_t count)
{
    CHECK(glDrawArrays(GL_LINES, static_cast<GLint>(offset), static_cast<GLsizei>(count)));
}

void OGLRenderDevice::drawTrianglesIndexed(std::size_t offset, std::size_t count)
{
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(count), static_cast<GLenum>(mCurrentIndexFormat),
                   reinterpret_cast<const void*>(offset * mCurrentIndexSz));
}

void OGLRenderDevice::drawTrianglesInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount)
{
    glDrawArraysInstanced(GL_TRIANGLES, static_cast<GLint>(offset), static_cast<GLsizei>(count),
                          static_cast<GLsizei>(instanceCount));
}

void OGLRenderDevice::drawTrianglesIndexedInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount)
{
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(count), static_cast<GLenum>(mCurrentIndexFormat),
                            reinterpret_cast<const void*>(offset * mCurrentIndexSz),
                            static_cast<GLsizei>(instanceCount));
}

void OGLRenderDevice::dispatchCompute(std::size_t x, std::size_t y, std::size_t z)
{
#ifndef __EMSCRIPTEN__
    CHECK(glDispatchCompute(static_cast<GLuint>(x), static_cast<GLuint>(y), static_cast<GLuint>(z)));
#else
    (void)x;
    (void)y;
    (void)z;
    CUBOS_ERROR("Compute shaders are not supported on Emscripten");
#endif
}

void OGLRenderDevice::memoryBarrier(MemoryBarriers barriers)
{
#ifndef __EMSCRIPTEN__
    GLbitfield barrier = 0;
    if ((barriers & MemoryBarriers::VertexBuffer) != MemoryBarriers::None)
    {
        barrier |= GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT;
    }
    if ((barriers & MemoryBarriers::IndexBuffer) != MemoryBarriers::None)
    {
        barrier |= GL_ELEMENT_ARRAY_BARRIER_BIT;
    }
    if ((barriers & MemoryBarriers::ConstantBuffer) != MemoryBarriers::None)
    {
        barrier |= GL_UNIFORM_BARRIER_BIT;
    }
    if ((barriers & MemoryBarriers::ImageAccess) != MemoryBarriers::None)
    {
        barrier |= GL_SHADER_IMAGE_ACCESS_BARRIER_BIT;
    }
    if ((barriers & MemoryBarriers::TextureAccess) != MemoryBarriers::None)
    {
        barrier |= GL_TEXTURE_FETCH_BARRIER_BIT;
    }
    if ((barriers & MemoryBarriers::Framebuffer) != MemoryBarriers::None)
    {
        barrier |= GL_FRAMEBUFFER_BARRIER_BIT;
    }
    CHECK(glMemoryBarrier(barrier));
#else
    (void)barriers;
    CUBOS_ERROR("Compute shaders are not supported on Emscripten");
#endif
}

void OGLRenderDevice::setViewport(int x, int y, int w, int h)
{
    CHECK(glViewport(x, y, w, h));
}

void OGLRenderDevice::setScissor(int x, int y, int w, int h)
{
    CHECK(glScissor(x, y, w, h));
}

int OGLRenderDevice::getProperty(Property prop)
{
    GLint major;
    GLint minor;

    switch (prop)
    {
    case Property::MaxAnisotropy:
        if (GLAD_GL_EXT_texture_filter_anisotropic == 0)
        {
            return 1;
        }
        else
        {
            GLfloat val;
            CHECK(glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &val));
            return static_cast<int>(val);
        }

    case Property::ComputeSupported:
#ifdef __EMSCRIPTEN__
        (void)major;
        (void)minor;
        return 0;
#else
        CHECK(glGetIntegerv(GL_MAJOR_VERSION, &major));
        CHECK(glGetIntegerv(GL_MINOR_VERSION, &minor));
        return (major >= 4 && minor >= 3) ? 1 : 0;
#endif

    default:
        return -1;
    }
}
