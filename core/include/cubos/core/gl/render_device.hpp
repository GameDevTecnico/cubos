/// @file
/// @brief Class @ref cubos::core::gl::RenderDevice and related types.
/// @ingroup core-gl

#pragma once

#include <memory>
#include <variant>

#include <glm/glm.hpp>

#include <cubos/core/api.hpp>

/// @brief Maximum number of render targets that can be set on a framebuffer.
/// @todo Make this a compile-time constant instead of a macro.
/// @ingroup core-gl
#define CUBOS_CORE_GL_MAX_FRAMEBUFFER_RENDER_TARGET_COUNT 8

/// @brief Maximum number of textures on a 2D texture array.
/// @todo Make this a compile-time constant instead of a macro.
/// @ingroup core-gl
#define CUBOS_CORE_GL_MAX_TEXTURE_2D_ARRAY_SIZE 256

/// @brief Maximum number of cube maps on a cubemap array.
/// @todo Make this a compile-time constant instead of a macro.
/// @ingroup core-gl
#define CUBOS_CORE_GL_MAX_CUBEMAP_ARRAY_SIZE 256

/// @brief Maximum mip level count.
/// @todo Make this a compile-time constant instead of a macro.
/// @ingroup core-gl
#define CUBOS_CORE_GL_MAX_MIP_LEVEL_COUNT 8

/// @brief Maximum constant buffer element name size.
/// @todo Make this a compile-time constant instead of a macro.
/// @ingroup core-gl
#define CUBOS_CORE_GL_MAX_CONSTANT_BUFFER_ELEMENT_NAME_SIZE 32

/// @brief Maximum constant buffer element count.
/// @todo Make this a compile-time constant instead of a macro.
/// @ingroup core-gl
#define CUBOS_CORE_GL_MAX_CONSTANT_BUFFER_ELEMENT_COUNT 32

/// @brief Maximum number of vertex array elements.
/// @todo Make this a compile-time constant instead of a macro.
/// @ingroup core-gl
#define CUBOS_CORE_GL_MAX_VERTEX_ARRAY_ELEMENT_COUNT 8

/// @brief Maximum number of buffers on a vertex array.
/// @todo Make this a compile-time constant instead of a macro.
/// @ingroup core-gl
#define CUBOS_CORE_GL_MAX_VERTEX_ARRAY_BUFFER_COUNT 8

namespace cubos::core::gl
{
    namespace impl
    {
        class Framebuffer;
        class RasterState;
        class DepthStencilState;
        class BlendState;

        class Sampler;
        class Texture1D;
        class Texture2D;
        class Texture2DArray;
        class Texture3D;
        class CubeMap;
        class CubeMapArray;

        class ConstantBuffer;
        class IndexBuffer;
        class VertexBuffer;
        class VertexArray;

        class ShaderStage;
        class ShaderPipeline;
        class ShaderBindingPoint;
    } // namespace impl

    /// @brief Handle to a framebuffer.
    /// @see @ref impl::Framebuffer - framebuffer interface.
    /// @ingroup core-gl
    using Framebuffer = std::shared_ptr<impl::Framebuffer>;

    /// @brief Handle to a rasterizer state.
    /// @see @ref impl::RasterState - rasterizer state interface.
    /// @ingroup core-gl
    using RasterState = std::shared_ptr<impl::RasterState>;

    /// @brief Handle to a depth stencil state.
    /// @see @ref impl::DepthStencilState - depth stencil state interface.
    /// @ingroup core-gl
    using DepthStencilState = std::shared_ptr<impl::DepthStencilState>;

    /// @brief Handle to a blend state.
    /// @see @ref impl::BlendState - blend state interface.
    /// @ingroup core-gl
    using BlendState = std::shared_ptr<impl::BlendState>;

    /// @brief Handle to a sampler.
    /// @see @ref impl::Sampler - sampler interface.
    /// @ingroup core-gl
    using Sampler = std::shared_ptr<impl::Sampler>;

    /// @brief Handle to a 1D texture.
    /// @see @ref impl::Texture1D - 1D texture interface.
    /// @ingroup core-gl
    using Texture1D = std::shared_ptr<impl::Texture1D>;

    /// @brief Handle to a 2D texture.
    /// @see @ref impl::Texture2D - 2D texture interface.
    /// @ingroup core-gl
    using Texture2D = std::shared_ptr<impl::Texture2D>;

    /// @brief Handle to a 2D texture array.
    /// @see @ref impl::Texture2DArray - 2D texture array interface.
    /// @ingroup core-gl
    using Texture2DArray = std::shared_ptr<impl::Texture2DArray>;

    /// @brief Handle to a 3D texture.
    /// @see @ref impl::Texture3D - 3D texture interface.
    /// @ingroup core-gl
    using Texture3D = std::shared_ptr<impl::Texture3D>;

    /// @brief Handle to a cube map.
    /// @see @ref impl::CubeMap - cube map interface.
    /// @ingroup core-gl
    using CubeMap = std::shared_ptr<impl::CubeMap>;

    /// @brief Handle to a cube map array.
    /// @see @ref impl::CubeMapArray - cube map array interface.
    /// @ingroup core-gl
    using CubeMapArray = std::shared_ptr<impl::CubeMapArray>;

    /// @brief Handle to a constant buffer.
    /// @see @ref impl::ConstantBuffer - constant buffer interface.
    /// @ingroup core-gl
    using ConstantBuffer = std::shared_ptr<impl::ConstantBuffer>;

    /// @brief Handle to an index buffer.
    /// @see @ref impl::IndexBuffer - index buffer interface.
    /// @ingroup core-gl
    using IndexBuffer = std::shared_ptr<impl::IndexBuffer>;

    /// @brief Handle to a vertex buffer.
    /// @see impl::VertexBuffer - vertex buffer interface.
    /// @ingroup core-gl
    using VertexBuffer = std::shared_ptr<impl::VertexBuffer>;

    /// @brief Handle to a vertex array.
    /// @see @ref impl::VertexArray - vertex array interface.
    /// @ingroup core-gl
    using VertexArray = std::shared_ptr<impl::VertexArray>;

    /// @brief Handle to a shader stage.
    /// @see @ref impl::ShaderStage - shader stage interface.
    /// @ingroup core-gl
    using ShaderStage = std::shared_ptr<impl::ShaderStage>;

    /// @brief Handle to a shader pipeline.
    /// @see @ref impl::ShaderPipeline - shader pipeline interface.
    /// @ingroup core-gl
    using ShaderPipeline = std::shared_ptr<impl::ShaderPipeline>;

    /// @brief Handle to a shader binding point.
    /// @see @ref impl::ShaderBindingPoint - shader binding point interface.
    /// @ingroup core-gl
    using ShaderBindingPoint = impl::ShaderBindingPoint*;

    /// @brief Render device properties that can be queried at runtime.
    /// @see @ref RenderDevice::getProperty().
    /// @ingroup core-gl
    enum class Property
    {
        /// @brief Specifies the upper bound of anisotropic filtering.
        MaxAnisotropy,

        /// @brief Specifies whether compute shaders and memory barriers are supported (0 or 1).
        ComputeSupported,
    };

    /// @brief Usage mode for buffers and textures.
    /// @ingroup core-gl
    enum class Usage
    {
        Default,
        Dynamic,
        Static,
    };

    /// @brief Data type.
    /// @ingroup core-gl
    enum class Type
    {
        Byte,    ///< 8 bits signed integer.
        Short,   ///< 16 bits signed integer.
        Int,     ///< 32 bits signed integer.
        UByte,   ///< 8 bits unsigned integer.
        UShort,  ///< 16 bits unsigned integer.
        UInt,    ///< 32 bits unsigned integer.
        NByte,   ///< Normalized 8 bits signed integer.
        NShort,  ///< Normalized 16 bits signed integer.
        NUByte,  ///< Normalized 8 bits unsigned integer.
        NUShort, ///< Normalized 16 bits unsigned integer.
        Float    ///< 32 bits floating point.
    };

    /// @brief Index format.
    /// @ingroup core-gl
    enum class IndexFormat
    {
        UShort, ///< 16 bits unsigned integer.
        UInt,   ///< 32 bits unsigned integer.
    };

    /// @brief Texture format.
    /// @ingroup core-gl
    enum class TextureFormat
    {
        R8SNorm,     ///< 1 channel 8 bits normalized signed integer.
        R16SNorm,    ///< 1 channel 16 bits normalized signed integer.
        RG8SNorm,    ///< 2 channel 8 bits normalized signed integer.
        RG16SNorm,   ///< 2 channel 16 bits normalized signed integer.
        RGBA8SNorm,  ///< 4 channel 8 bits normalized signed integer.
        RGBA16SNorm, ///< 4 channel 16 bits normalized signed integer.
        R8UNorm,     ///< 1 channel 8 bits normalized unsigned integer.
        R16UNorm,    ///< 1 channel 16 bits normalized unsigned integer.
        RG8UNorm,    ///< 2 channel 8 bits normalized unsigned integer.
        RG16UNorm,   ///< 2 channel 16 bits normalized unsigned integer.
        RGBA8UNorm,  ///< 4 channel 8 bits normalized unsigned integer.
        RGBA16UNorm, ///< 4 channel 16 bits normalized unsigned integer.
        R8SInt,      ///< 1 channel 8 bits signed integer.
        R16SInt,     ///< 1 channel 16 bits signed integer.
        RG8SInt,     ///< 2 channel 8 bits signed integer.
        RG16SInt,    ///< 2 channel 16 bits signed integer.
        RGBA8SInt,   ///< 4 channel 8 bits signed integer.
        RGBA16SInt,  ///< 4 channel 16 bits signed integer.
        R8UInt,      ///< 1 channel 8 bits unsigned integer.
        R16UInt,     ///< 1 channel 16 bits unsigned integer.
        RG8UInt,     ///< 2 channel 8 bits unsigned integer.
        RG16UInt,    ///< 2 channel 16 bits unsigned integer.
        RGBA8UInt,   ///< 4 channel 8 bits unsigned integer.
        RGBA16UInt,  ///< 4 channel 16 bits unsigned integer.
        R16Float,    ///< 1 channel 16 bits floating point.
        R32Float,    ///< 1 channel 32 bits floating point.
        RG16Float,   ///< 2 channel 16 bits floating point.
        RG32Float,   ///< 2 channel 32 bits floating point.
        RGB16Float,  ///< 3 channel 16 bits floating point.
        RGB32Float,  ///< 3 channel 32 bits floating point.
        RGBA16Float, ///< 4 channel 16 bits floating point.
        RGBA32Float, ///< 4 channel 32 bits floating point.

        Depth16,         ///< 16 depth bits.
        Depth32,         ///< 32 depth bits.
        Depth24Stencil8, ///< 24 depth bits and 8 stencil bits.
        Depth32Stencil8, ///< 32 depth bits and 8 stencil bits.
    };

    /// @brief Texture address mode.
    /// @ingroup core-gl
    enum class AddressMode
    {
        Repeat,
        Mirror,
        Clamp,
        Border,
    };

    /// @brief Texture filter type.
    /// @ingroup core-gl
    enum class TextureFilter
    {
        None,
        Nearest,
        Linear,
    };

    /// @brief Triangle winding mode.
    /// @ingroup core-gl
    enum class Winding
    {
        CW, ///< Clockwise.
        CCW ///< Counterclockwise.
    };

    /// @brief Specifies a face.
    /// @ingroup core-gl
    enum class Face
    {
        Front,
        Back,
        FrontAndBack,
    };

    /// @brief Rasterizer mode.
    /// @ingroup core-gl
    enum class RasterMode
    {
        Wireframe,
        Fill,
    };

    /// @brief Comparison function.
    /// @ingroup core-gl
    enum class Compare
    {
        Never,
        Less,
        LEqual,
        Greater,
        GEqual,
        Equal,
        NEqual,
        Always,
    };

    /// @brief Stencil action.
    /// @ingroup core-gl
    enum class StencilAction
    {
        Zero,
        Keep,
        Replace,
        Increment,
        IncrementWrap,
        Decrement,
        DecrementWrap,
        Invert,
    };

    /// @brief Blend factor.
    /// @ingroup core-gl
    enum class BlendFactor
    {
        Zero,
        One,
        SrcColor,
        InvSrcColor,
        DstColor,
        InvDstColor,
        SrcAlpha,
        InvSrcAlpha,
        DstAlpha,
        InvDstAlpha,
    };

    /// @brief Blend operation.
    /// @ingroup core-gl
    enum class BlendOp
    {
        Add,
        Substract,
        RevSubstract,
        Min,
        Max,
    };

    /// @brief Shader stage type.
    /// @ingroup core-gl
    enum class Stage
    {
        Vertex,
        Geometry,
        Pixel,
        Compute,
    };

    /// @brief Cube map face.
    /// @ingroup core-gl
    enum class CubeFace
    {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5,
    };

    /// @brief Memory barrier flags for synchronization.
    /// @ingroup core-gl
    enum class MemoryBarriers
    {
        /// @brief Utility flag to indicate that no memory barrier is set.
        None = 0,

        /// @brief If set, data sourced from vertex buffers after the barrier will reflect the data
        /// written by shaders prior to the barrier.
        VertexBuffer = 1,

        /// @brief If set, data sourced from index buffers after the barrier will reflect the data
        /// written by shaders prior to the barrier.
        IndexBuffer = 2,

        /// @brief If set, data sourced from constant buffers after the barrier will reflect the
        /// data written by shaders prior to the barrier.
        ConstantBuffer = 4,

        /// @brief If set, memory accesses using shader image load, store, and atomic built-in
        /// functions issued after the barrier will reflect the data written by shaders prior to
        /// the barrier.
        ImageAccess = 8,

        /// @brief If set, texture accesses from shaders after the barrier will reflect the data
        /// written by shaders prior to the barrier.
        TextureAccess = 16,

        /// @brief If set, reads and writes via framebuffer objects after the barrier will reflect
        /// the data written by shaders prior to the barrier.
        Framebuffer = 32,

        /// @brief Utility flag to set all memory barriers.
        All = VertexBuffer | IndexBuffer | ConstantBuffer | ImageAccess | TextureAccess | Framebuffer,
    };

    /// @brief Access mode for a resource.
    /// @ingroup core-gl
    enum class Access
    {
        Read,      ///< Read access.
        Write,     ///< Write access.
        ReadWrite, ///< Read and write access.
    };

    /// @brief Describes a framebuffer.
    /// @ingroup core-gl
    struct CUBOS_CORE_API FramebufferDesc
    {
        /// @brief Describes a cube map target.
        struct CubeMapTarget
        {
            CubeMap handle; ///< Cube map handle.
            CubeFace face;  ///< Cube map face which will be used as target.
        };

        /// @brief Describes a 2D texture target.
        struct Texture2DTarget
        {
            Texture2D handle; ///< Texture handle.
        };

        /// @brief Describes a cube map array target.
        struct CubeMapArrayTarget
        {
            CubeMapArray handle; ///< Cube map array handle.
        };

        /// @brief Describes a 2D texture array target.
        struct Texture2DArrayTarget
        {
            Texture2DArray handle; ///< Texture array handle.
        };

        /// @brief Possible types of targets.
        enum class TargetType
        {
            CubeMap,
            Texture2D,
            CubeMapArray,
            Texture2DArray
        };

        /// @brief Describes a framebuffer target.
        struct CUBOS_CORE_API FramebufferTarget
        {
            uint32_t mipLevel = 0; ///< Mip level of the target which will be set as a render target.

            TargetType getTargetType() const;
            bool isSet() const;

            const CubeMapTarget& getCubeMapTarget() const;

            const Texture2DTarget& getTexture2DTarget() const;

            const Texture2DArrayTarget& getTexture2DArrayTarget() const;

            const CubeMapArrayTarget& getCubeMapArrayTarget() const;

            void setCubeMapTarget(const CubeMap& handle, CubeFace face);

            void setTexture2DTarget(const Texture2D& handle);

            void setTexture2DArrayTarget(const Texture2DArray& handle);

            void setCubeMapArrayTarget(const CubeMapArray& handle);

        private:
            bool mSet = false;
            TargetType mType; ///< Type of the currently set target.

            std::variant<CubeMapTarget, Texture2DTarget, CubeMapArrayTarget, Texture2DArrayTarget> mTarget;
        } targets[CUBOS_CORE_GL_MAX_FRAMEBUFFER_RENDER_TARGET_COUNT]; ///< Render targets.

        uint32_t targetCount = 1;       ///< Number of render targets.
        FramebufferTarget depthStencil; ///< Optional depth stencil target.
    };

    /// @brief Decribes a rasterizer state.
    /// @ingroup core-gl
    struct RasterStateDesc
    {
        bool cullEnabled = false;                 ///< Is face culling enabled?
        Face cullFace = Face::Back;               ///< Face that will be culled.
        Winding frontFace = Winding::CCW;         ///< Front face winding.
        RasterMode rasterMode = RasterMode::Fill; ///< Rasterizer mode.
        bool scissorEnabled = false;              ///< Is scissor test enabled?
    };

    /// @brief Describes a depth stencil state.
    /// @ingroup core-gl
    struct DepthStencilStateDesc
    {
        /// @brief Decribes a depth state.
        struct Depth
        {
            bool enabled = false;            ///< Enable depth checks?
            bool writeEnabled = false;       ///< Write to depth?
            float near = 0.0F;               ///< Depth near value.
            float far = 1.0F;                ///< Depth far value.
            Compare compare = Compare::Less; ///< Depth comparison function.
        };

        /// @brief Describes a stencil state.
        struct Stencil
        {
            /// @brief Decribes a stencil face.
            struct Face
            {
                Compare compare = Compare::Always;             ///< Stencil comparison function.
                StencilAction fail = StencilAction::Keep;      ///< Stencil comparison fail action.
                StencilAction pass = StencilAction::Keep;      ///< Stencil comparison pass action.
                StencilAction depthFail = StencilAction::Keep; ///< Stencil depth comparison fail action.
            };

            bool enabled = false;     ///< Is stencil enabled?
            uint32_t ref = 0;         ///< Stencil ref value used on stencil comparison functions.
            uint8_t readMask = 0xFF;  ///< Stencil read mask.
            uint8_t writeMask = 0xFF; ///< Stencil write mask.

            Face backFace;  ///< Stencil back face options.
            Face frontFace; ///< Stencil front face options.
        };

        Depth depth;     ///< Depth state options.
        Stencil stencil; ///< Depth stencil options.
    };

    /// @brief Describes a blend state.
    /// @ingroup core-gl
    struct BlendStateDesc
    {
        bool blendEnabled = false; ///< Enable blending?

        struct
        {
            BlendFactor src = BlendFactor::SrcAlpha;    ///< Color blend source factor.
            BlendFactor dst = BlendFactor::InvSrcAlpha; ///< Color blend destination factor.
            BlendOp op = BlendOp::Add;                  ///< Color blend operation.
        } color;                                        ///< Color blend state.

        struct
        {
            BlendFactor src = BlendFactor::One;  ///< Alpha blend source factor.
            BlendFactor dst = BlendFactor::Zero; ///< Alpha blend destination factor.
            BlendOp op = BlendOp::Add;           ///< Alpha blend operation.
        } alpha;                                 ///< Alpha blend state.
    };

    /// @brief Describes a sampler.
    /// @ingroup core-gl
    struct SamplerDesc
    {
        float borderColor[4] = {0.0F}; ///< Border color applied when the address mode is AddressMode::Border.

        TextureFilter minFilter = TextureFilter::Nearest; ///< Minifying filter.
        TextureFilter magFilter = TextureFilter::Nearest; ///< Magnifying filter.
        TextureFilter mipmapFilter = TextureFilter::None; ///< Set to None to disable mipmapping.

        AddressMode addressU = AddressMode::Clamp; ///< Texture adress mode on coordinate U.
        AddressMode addressV = AddressMode::Clamp; ///< Texture adress mode on coordinate V.
        AddressMode addressW = AddressMode::Clamp; ///< Texture adress mode on coordinate W.

        std::size_t maxAnisotropy = 1; ///< Max anisotropy for filtering. Limited to Property::MaxAnisotropy.
    };

    /// @brief Describes a 1D texture.
    /// @ingroup core-gl
    struct Texture1DDesc
    {
        const void* data[CUBOS_CORE_GL_MAX_MIP_LEVEL_COUNT] = {}; ///< Optional initial texture data.
        std::size_t mipLevelCount = 1;                            ///< Number of mip levels.
        std::size_t width;                                        ///< Texture width.
        Usage usage;                                              ///< Texture usage mode.
        TextureFormat format;                                     ///< Texture format.
    };

    /// @brief Describes a 2D texture.
    /// @ingroup core-gl
    struct Texture2DDesc
    {
        const void* data[CUBOS_CORE_GL_MAX_MIP_LEVEL_COUNT] = {}; ///< Optional initial texture data.
        std::size_t mipLevelCount = 1;                            ///< Number of mip levels.
        std::size_t width;                                        ///< Texture width.
        std::size_t height;                                       ///< Texture height.
        Usage usage;                                              ///< Texture usage mode.
        TextureFormat format;                                     ///< Texture format.
    };

    /// @brief Describes a 2D texture array.
    /// @ingroup core-gl
    struct Texture2DArrayDesc
    {
        const void* data[CUBOS_CORE_GL_MAX_TEXTURE_2D_ARRAY_SIZE][CUBOS_CORE_GL_MAX_MIP_LEVEL_COUNT] =
            {};                        ///< Optional initial texture data.
        std::size_t mipLevelCount = 1; ///< Number of mip levels.
        std::size_t width;             ///< Texture width.
        std::size_t height;            ///< Texture height.
        std::size_t size;              ///< Number of 2D Textures contained in the array.
        Usage usage;                   ///< Texture usage mode.
        TextureFormat format;          ///< Texture format.
    };

    /// @brief Describes a 3D texture.
    /// @ingroup core-gl
    struct Texture3DDesc
    {
        const void* data[CUBOS_CORE_GL_MAX_MIP_LEVEL_COUNT] = {}; ///< Optional initial texture data.
        std::size_t mipLevelCount = 1;                            ///< Number of mip levels.
        std::size_t width;                                        ///< Texture width.
        std::size_t height;                                       ///< Texture height.
        std::size_t depth;                                        ///< Texture depth.
        Usage usage;                                              ///< Texture usage mode.
        TextureFormat format;                                     ///< Texture format.
    };

    /// @brief Describes a cube map.
    /// @ingroup core-gl
    struct CubeMapDesc
    {
        const void* data[6][CUBOS_CORE_GL_MAX_MIP_LEVEL_COUNT] =
            {};                        ///< Optional initial cube map data, indexed using CubeFace.
        std::size_t mipLevelCount = 1; ///< Number of mip levels.
        std::size_t width;             ///< Cube map face width.
        std::size_t height;            ///< Cube map face height.
        Usage usage;                   ///< Texture usage mode.
        TextureFormat format;          ///< Texture format.
    };

    /// @brief Describes a cube map array.
    /// @ingroup core-gl
    struct CubeMapArrayDesc
    {
        const void* data[CUBOS_CORE_GL_MAX_CUBEMAP_ARRAY_SIZE][6][CUBOS_CORE_GL_MAX_MIP_LEVEL_COUNT] =
            {};                        ///< Optional initial cube map data, indexed using CubeFace.
        std::size_t mipLevelCount = 1; ///< Number of mip levels.
        std::size_t width;             ///< Cube map face width.
        std::size_t height;            ///< Cube map face height.
        std::size_t size;              ///< Number of cube maps contained in the array.
        Usage usage;                   ///< Texture usage mode.
        TextureFormat format;          ///< Texture format.
    };

    /// @brief Describes an element in a constant buffer.
    /// @ingroup core-gl
    struct ConstantBufferElement
    {
        char name[CUBOS_CORE_GL_MAX_CONSTANT_BUFFER_ELEMENT_NAME_SIZE]; ///< Element name.

        std::size_t offset; ///< Offset of the element in the buffer.
        std::size_t size;   ///< Number of values in the element if it is an array. If it isn't, this is set to 1.
        std::size_t stride; ///< Stride between each element in the array, if the element is an array.
    };

    /// @brief Describes the structure of a constant buffer.
    /// @ingroup core-gl
    struct ConstantBufferStructure
    {
        std::size_t size;         ///< Size of the buffer in bytes.
        std::size_t elementCount; ///< Number of elements.
        ConstantBufferElement elements[CUBOS_CORE_GL_MAX_CONSTANT_BUFFER_ELEMENT_COUNT]; ///< Constant buffer elements.
    };

    /// @brief Describes a vertex element.
    /// @ingroup core-gl
    struct VertexElement
    {
        const char* name;        ///< Element name.
        Type type = Type::Float; ///< Element type.
        std::size_t size;        ///< Number of components in the vertex element (1, 2, 3 or 4).

        struct
        {
            std::size_t stride = 0; ///< Stride between each element in memory.
            std::size_t offset = 0; ///< Offset of the first element in the buffer.
            std::size_t index = 0;  ///< Index of the vertex buffer where the element is stored.
        } buffer;                   ///< Vertex buffer description.
    };

    /// @brief Describes a vertex array.
    /// @ingroup core-gl
    struct VertexArrayDesc
    {
        std::size_t elementCount = 0;                                         ///< Number of vertex elements.
        VertexElement elements[CUBOS_CORE_GL_MAX_VERTEX_ARRAY_ELEMENT_COUNT]; ///< Vertex elements.
        VertexBuffer buffers[CUBOS_CORE_GL_MAX_VERTEX_ARRAY_BUFFER_COUNT];    ///< Vertex buffers.
        ShaderPipeline shaderPipeline; ///< Shader pipeline used with the vertex array.
    };

    /// @brief Interface used to wrap low-level rendering APIs such as OpenGL.
    ///
    /// Using this interface, the engine never directly interacts with *OpenGL* or any other
    /// low-level rendering API. This allows use to use different rendering APIs without having
    /// to change the engine code, which is useful for porting the engine to different platforms.
    ///
    /// @see @ref io::Window is responsible for creating a render device for itself.
    /// @ingroup core-gl
    class CUBOS_CORE_API RenderDevice
    {
    public:
        virtual ~RenderDevice() = default;
        RenderDevice() = default;

        /// @name Forbid any kind of copying.
        /// @{
        RenderDevice(const RenderDevice&) = delete;
        RenderDevice& operator=(const RenderDevice&) = delete;
        /// @}

        /// @brief Creates a new framebuffer.
        /// @param desc Framebuffer description.
        /// @return Framebuffer handle, or nullptr on failure.
        virtual Framebuffer createFramebuffer(const FramebufferDesc& desc) = 0;

        /// @brief Sets the current framebuffer.
        /// @param fb Framebuffer handle.
        virtual void setFramebuffer(Framebuffer fb) = 0;

        /// @brief Creates a new rasterizer state.
        /// @param desc Rasterizer state description.
        /// @return Rasterizer state handle, or nullptr on failure.
        virtual RasterState createRasterState(const RasterStateDesc& desc) = 0;

        /// @brief Sets the current rasterizer state.
        /// @param rs Rasterizer state handle.
        virtual void setRasterState(RasterState rs) = 0;

        /// @brief Creates a new depth stencil state.
        /// @param desc Depth stencil state description.
        /// @return Depth stencil state handle, or nullptr on failure.
        virtual DepthStencilState createDepthStencilState(const DepthStencilStateDesc& desc) = 0;

        /// @brief Sets the current depth stencil state.
        /// @param dss Depth stencil state handle.
        virtual void setDepthStencilState(DepthStencilState dss) = 0;

        /// @brief Creates a new blend state.
        /// @param desc Blend state description.
        /// @return Blend state handle, or nullptr on failure.
        virtual BlendState createBlendState(const BlendStateDesc& desc) = 0;

        /// @brief Sets the current blend state.
        /// @param bs Blend state handle.
        virtual void setBlendState(BlendState bs) = 0;

        /// @brief Creates a new texture sampler.
        /// @param desc Sampler description.
        /// @return Sampler handle, or nullptr on failure.
        virtual Sampler createSampler(const SamplerDesc& desc) = 0;

        /// @brief Creates a new 1D texture.
        /// @param desc 1D texture description.
        /// @return Texture handle, or nullptr on failure.
        virtual Texture1D createTexture1D(const Texture1DDesc& desc) = 0;

        /// @brief Creates a new 2D texture.
        /// @param desc 2D texture description.
        /// @return Texture handle, or nullptr on failure.
        virtual Texture2D createTexture2D(const Texture2DDesc& desc) = 0;

        /// @brief Creates a new 2D texture array.
        /// @param desc 2D texture array description.
        /// @return Texture array handle, or nullptr on failure.
        virtual Texture2DArray createTexture2DArray(const Texture2DArrayDesc& desc) = 0;

        /// @brief Creates a new 3D texture.
        /// @param desc 3D texture description.
        /// @return Texture handle, or nullptr on failure.
        virtual Texture3D createTexture3D(const Texture3DDesc& desc) = 0;

        /// @brief Creates a new cube map.
        /// @param desc Cube map description.
        /// @return Cube map handle, or nullptr on failure.
        virtual CubeMap createCubeMap(const CubeMapDesc& desc) = 0;

        /// @brief Creates a new cube map array.
        /// @param desc Cube map array description.
        /// @return Cube map array handle, or nullptr on failure.
        virtual CubeMapArray createCubeMapArray(const CubeMapArrayDesc& desc) = 0;

        /// @brief Creates a new constant buffer.
        /// @param size Size in bytes.
        /// @param data Initial data, can be nullptr.
        /// @param usage Usage which the buffer will have.
        /// @return Constant buffer handle, or nullptr on failure.
        virtual ConstantBuffer createConstantBuffer(std::size_t size, const void* data, Usage usage) = 0;

        /// @brief Creates a new index buffer.
        /// @param size Size in bytes.
        /// @param data Initial data, can be nullptr.
        /// @param format Index format.
        /// @param usage Usage which the buffer will have.
        /// @return Index buffer handle, or nullptr on failure.
        virtual IndexBuffer createIndexBuffer(std::size_t size, const void* data, IndexFormat format, Usage usage) = 0;

        /// @brief Sets the current index buffer.
        /// @param ib Index buffer handle.
        virtual void setIndexBuffer(IndexBuffer ib) = 0;

        /// @brief Creates a new vertex buffer.
        /// @param size Size in bytes.
        /// @param data Initial data, can be nullptr.
        /// @param usage Usage which the buffer will have.
        /// @return Vertex buffer handle, or nullptr on failure.
        virtual VertexBuffer createVertexBuffer(std::size_t size, const void* data, Usage usage) = 0;

        /// @brief Creates a new vertex array.
        /// @param desc Vertex array description.
        /// @return Vertex array handle, or nullptr on failure.
        virtual VertexArray createVertexArray(const VertexArrayDesc& desc) = 0;

        /// @brief Sets the current vertex array.
        /// @param va Vertex array handle.
        virtual void setVertexArray(VertexArray va) = 0;

        /// @brief Creates a new shader stage from GLSL source code.
        /// @note Compute shaders are unsupported on some platforms. Support can be queried using
        /// getProperty().
        /// @param stage Shader stage.
        /// @param src GLSL source code.
        /// @return Shader stage handle, or nullptr on failure.
        virtual ShaderStage createShaderStage(Stage stage, const char* src) = 0;

        /// @brief Creates a new shader pipeline from vertex and pixel shaders.
        /// @param vs Vertex shader stage.
        /// @param ps Pixel shader stage.
        /// @return Shader pipeline handle, or nullptr on failure.
        virtual ShaderPipeline createShaderPipeline(ShaderStage vs, ShaderStage ps) = 0;

        /// @brief Creates a new shader pipeline from vertex, pixel and geometry shaders.
        /// @param vs Vertex shader stage.
        /// @param gs Geometry shader stage.
        /// @param ps Pixel shader stage.
        /// @return Shader pipeline handle, or nullptr on failure.
        virtual ShaderPipeline createShaderPipeline(ShaderStage vs, ShaderStage gs, ShaderStage ps) = 0;

        /// @brief Creates a new shader pipeline from a compute shader. Unsupported on some
        /// platforms.
        /// @note Support can be queried using getProperty().
        /// @param cs Compute shader stage.
        /// @return Shader pipeline handle, or nullptr on failure.
        virtual ShaderPipeline createShaderPipeline(ShaderStage cs) = 0;

        /// @brief Sets the current shader pipeline used for rendering.
        /// @param pipeline Shader pipeline handle.
        virtual void setShaderPipeline(ShaderPipeline pipeline) = 0;

        /// @brief Clears the color buffer bit on the current framebuffer to a specific color.
        /// @param r Red component.
        /// @param g Green component.
        /// @param b Blue component.
        /// @param a Alpha component.
        virtual void clearColor(float r, float g, float b, float a) = 0;

        /// @brief Clears the color buffer of a specific target on the current framebuffer to a
        /// specific color.
        /// @param target Target index.
        /// @param r Red component.
        /// @param g Green component.
        /// @param b Blue component.
        /// @param a Alpha component.
        virtual void clearTargetColor(std::size_t target, float r, float g, float b, float a) = 0;

        /// @brief Clear the color buffer of a specific target on the current framebuffer to a
        /// specific color using integer values.
        /// @param target Target index.
        /// @param r Red component.
        /// @param g Green component.
        /// @param b Blue component.
        /// @param a Alpha component.
        virtual void clearTargetColor(std::size_t target, int r, int g, int b, int a) = 0;

        /// @brief Clears the depth buffer bit on the current framebuffer to a specific value.
        /// @param depth Depth value.
        virtual void clearDepth(float depth) = 0;

        /// @brief Clears the stencil buffer bit on the current framebuffer to a specific value.
        /// @param stencil Stencil value.
        virtual void clearStencil(int stencil) = 0;

        /// @brief Draws lines.
        /// @param offset Index of the first vertex to be drawn.
        /// @param count Number of vertices that will be drawn.
        virtual void drawLines(std::size_t offset, std::size_t count) = 0;

        /// @brief Draws tringles.
        /// @param offset Index of the first vertex to be drawn.
        /// @param count Number of vertices that will be drawn.
        virtual void drawTriangles(std::size_t offset, std::size_t count) = 0;

        /// @brief Draws tringles with an index buffer.
        /// @param offset Index of the first indice to be drawn.
        /// @param count Number of indices that will be drawn.
        virtual void drawTrianglesIndexed(std::size_t offset, std::size_t count) = 0;

        /// @brief Draws tringles multiple times.
        /// @param offset Index of the first vertex to be drawn.
        /// @param count Number of vertices that will be drawn.
        /// @param instanceCount Number of instances drawn.
        virtual void drawTrianglesInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount) = 0;

        /// @brief Draws tringles multiple times with an index buffer.
        /// @param offset Index of the first indice to be drawn.
        /// @param count Number of indices that will be drawn.
        /// @param instanceCount Number of instances drawn.
        virtual void drawTrianglesIndexedInstanced(std::size_t offset, std::size_t count,
                                                   std::size_t instanceCount) = 0;

        /// @brief Dispatches a compute pipeline.
        /// @param x X dimension of the work group.
        /// @param y Y dimension of the work group.
        /// @param z Z dimension of the work group.
        virtual void dispatchCompute(std::size_t x, std::size_t y, std::size_t z) = 0;

        /// @brief Defines a barrier ordering memory transactions. Unsupported on some platforms.
        ///
        /// This ensure that all memory transactions before the barrier are completed before the
        /// barrier is executed.
        ///
        /// @note Support can be queried using @ref getProperty().
        /// @param barriers Barriers to apply.
        virtual void memoryBarrier(MemoryBarriers barriers) = 0;

        /// @brief Sets the current viewport.
        /// @param x Bottom left viewport corner X coordinate.
        /// @param y Bottom left viewport corner Y coordinate.
        /// @param w Viewport width.
        /// @param h Viewport height.
        virtual void setViewport(int x, int y, int w, int h) = 0;

        /// @brief Sets the current scissor rectangle.
        /// @param x Bottom left scissor rectangle corner X coordinate.
        /// @param y Bottom left scissor rectangle corner Y coordinate.
        /// @param w Scissor rectangle width.
        /// @param h Scissor rectangle height.
        virtual void setScissor(int x, int y, int w, int h) = 0;

        /// @brief Gets a runtime property of the render device.
        /// @param prop Property name.
        virtual int getProperty(Property prop) = 0;
    };

    namespace impl
    {
        /// @brief Abstract framebuffer.
        class CUBOS_CORE_API Framebuffer
        {
        public:
            virtual ~Framebuffer() = default;

        protected:
            Framebuffer() = default;
        };

        /// @brief Abstract rasterizer state.
        class CUBOS_CORE_API RasterState
        {
        public:
            virtual ~RasterState() = default;

        protected:
            RasterState() = default;
        };

        /// @brief Abstract depth stencil state.
        class CUBOS_CORE_API DepthStencilState
        {
        public:
            virtual ~DepthStencilState() = default;

        protected:
            DepthStencilState() = default;
        };

        /// @brief Abstract blend state.
        class CUBOS_CORE_API BlendState
        {
        public:
            virtual ~BlendState() = default;

        protected:
            BlendState() = default;
        };

        /// @brief Abstract sampler.
        class CUBOS_CORE_API Sampler
        {
        public:
            virtual ~Sampler() = default;

        protected:
            Sampler() = default;
        };

        /// @brief Abstract 1D texture.
        class CUBOS_CORE_API Texture1D
        {
        public:
            virtual ~Texture1D() = default;

            /// @brief Updates the texture with new data, which must have the same format used when
            /// the texture was created.
            /// @param x Destination X coordinate.
            /// @param width Width of the section which will be updated.
            /// @param data Pointer to the new data.
            /// @param level Mip level to update.
            virtual void update(std::size_t x, std::size_t width, const void* data, std::size_t level = 0) = 0;

            /// @brief Generates mipmaps on this texture.
            virtual void generateMipmaps() = 0;

        protected:
            Texture1D() = default;
        };

        /// @brief Abstract 2D texture.
        class CUBOS_CORE_API Texture2D
        {
        public:
            virtual ~Texture2D() = default;

            /// @brief Updates the texture with new data, which must have the same format used when
            /// the texture was created.
            /// @param x Destination X coordinate.
            /// @param y Destination Y coordinate.
            /// @param width Width of the section which will be updated.
            /// @param height Height of the section which will be updated.
            /// @param data Pointer to the new data.
            /// @param level Mip level to update.
            virtual void update(std::size_t x, std::size_t y, std::size_t width, std::size_t height, const void* data,
                                std::size_t level = 0) = 0;

            /// @brief Reads texture data into a buffer, which must have the same format used when
            /// the texture was created.
            /// @param outputBuffer Buffer to write the data to.
            /// @param level Mip level to read.
            virtual void read(void* outputBuffer, std::size_t level = 0) = 0;

            /// @brief Generates mipmaps on this texture.
            virtual void generateMipmaps() = 0;

        protected:
            Texture2D() = default;
        };

        /// @brief Abstract 2D texture array.
        class CUBOS_CORE_API Texture2DArray
        {
        public:
            virtual ~Texture2DArray() = default;

            /// @brief Updates the texture with new data, which must have the same format used when
            /// the texture was created.
            /// @param x Destination X coordinate.
            /// @param y Destination Y coordinate.
            /// @param i Index of the destination texture within the array.
            /// @param width Width of the section which will be updated.
            /// @param height Height of the section which will be updated.
            /// @param data Pointer to the new data.
            /// @param level Mip level to update.
            virtual void update(std::size_t x, std::size_t y, std::size_t i, std::size_t width, std::size_t height,
                                const void* data, std::size_t level = 0) = 0;

            /// @brief Generates mipmaps on this texture.
            virtual void generateMipmaps() = 0;

        protected:
            Texture2DArray() = default;
        };

        /// @brief Abstract 3D texture.
        class CUBOS_CORE_API Texture3D
        {
        public:
            virtual ~Texture3D() = default;

            /// @brief Updates the texture with new data, which must have the same format used when
            /// the texture was created.
            /// @param x Destination X coordinate.
            /// @param y Destination Y coordinate.
            /// @param z Destination Z coordinate.
            /// @param width Width of the section which will be updated.
            /// @param height Height of the section which will be updated.
            /// @param depth Depth of the section which will be updated.
            /// @param data Pointer to the new data.
            /// @param level Mip level to update.
            virtual void update(std::size_t x, std::size_t y, std::size_t z, std::size_t width, std::size_t height,
                                std::size_t depth, const void* data, std::size_t level = 0) = 0;

            /// @brief Generates mipmaps on this texture.
            virtual void generateMipmaps() = 0;

        protected:
            Texture3D() = default;
        };

        /// @brief Abstract cube map.
        class CUBOS_CORE_API CubeMap
        {
        public:
            virtual ~CubeMap() = default;

            /// @brief Updates a cube map's face with new data, which must have the same format
            /// used when the cube map was created.
            /// @param x Destination X coordinate.
            /// @param y Destination Y coordinate.
            /// @param width Width of the section which will be updated.
            /// @param height Height of the section which will be updated.
            /// @param data Pointer to the new data.
            /// @param face Face to update.
            /// @param level Mip level to update.
            virtual void update(std::size_t x, std::size_t y, std::size_t width, std::size_t height, const void* data,
                                CubeFace face, std::size_t level = 0) = 0;

            /// @brief Generates mipmaps on this cube map.
            virtual void generateMipmaps() = 0;

        protected:
            CubeMap() = default;
        };

        /// @brief Abstract cube map.
        class CUBOS_CORE_API CubeMapArray
        {
        public:
            virtual ~CubeMapArray() = default;

            /// @brief Updates a cube map's face with new data, which must have the same format
            /// used when the cube map was created.
            /// @param x Destination X coordinate.
            /// @param y Destination Y coordinate.
            /// @param i Index of the destination texture within the array.
            /// @param width Width of the section which will be updated.
            /// @param height Height of the section which will be updated.
            /// @param data Pointer to the new data.
            /// @param face Face to update.
            /// @param level Mip level to update.
            virtual void update(std::size_t x, std::size_t y, std::size_t i, std::size_t width, std::size_t height,
                                const void* data, CubeFace face, std::size_t level = 0) = 0;

            /// @brief Generates mipmaps on this cube map.
            virtual void generateMipmaps() = 0;

        protected:
            CubeMapArray() = default;
        };

        /// @brief Abstract constant buffer.
        class CUBOS_CORE_API ConstantBuffer
        {
        public:
            virtual ~ConstantBuffer() = default;

            /// @brief Maps the constant buffer to a region in memory. Must be matched with a call
            /// to @ref unmap().
            /// @return Pointer to the memory region.
            virtual void* map() = 0;

            /// @brief Unmaps the constant buffer, updating it with data written to the mapped
            /// region.
            virtual void unmap() = 0;

        protected:
            ConstantBuffer() = default;
        };

        /// @brief Abstract index buffer.
        class CUBOS_CORE_API IndexBuffer
        {
        public:
            virtual ~IndexBuffer() = default;

            /// @brief Maps the index buffer to a region in memory. Must be matched with a call
            /// to @ref unmap().
            /// @return Pointer to the memory region.
            virtual void* map() = 0;

            /// @brief Unmaps the index buffer, updating it with data written to the mapped region.
            virtual void unmap() = 0;

        protected:
            IndexBuffer() = default;
        };

        /// @brief Abstract vertex buffer.
        class CUBOS_CORE_API VertexBuffer
        {
        public:
            virtual ~VertexBuffer() = default;

            /// @brief Maps the vertex buffer to a region in memory. Must be matched with a call
            /// to @ref unmap().
            /// @return Pointer to the memory region.
            virtual void* map() = 0;

            /// @brief Maps a region of the vertex buffer to a region in memory. Must be matched with a call to @ref
            /// unmap().
            /// @param offset Offset in bytes.
            /// @param length Length in bytes.
            /// @param synchronized Whether pending operations on the buffer should be synchronized prior to returning
            /// from this method.
            /// @return Pointer to the memory region.
            virtual void* map(std::size_t offset, std::size_t length, bool synchronized = true) = 0;

            /// @brief Unmaps the vertex buffer, updating it with data written to the mapped
            /// region.
            virtual void unmap() = 0;

        protected:
            VertexBuffer() = default;
        };

        /// @brief Abstract vertex array.
        class CUBOS_CORE_API VertexArray
        {
        public:
            virtual ~VertexArray() = default;

        protected:
            VertexArray() = default;
        };

        /// @brief Abstract shader stage.
        class CUBOS_CORE_API ShaderStage
        {
        public:
            virtual ~ShaderStage() = default;

            /// @brief Gets the shader stage type.
            /// @return Shader stage type.
            virtual Stage getType() = 0;

        protected:
            ShaderStage() = default;
        };

        /// @brief Abstract shader pipeline.
        class CUBOS_CORE_API ShaderPipeline
        {
        public:
            virtual ~ShaderPipeline() = default;

            /// @brief Gets a binding point from its name.
            /// @return Binding point, or nullptr if no binding point is found.
            virtual gl::ShaderBindingPoint getBindingPoint(const char* name) = 0;

        protected:
            ShaderPipeline() = default;
        };

        /// @brief Abstract shader binding point.
        class CUBOS_CORE_API ShaderBindingPoint
        {
        public:
            virtual ~ShaderBindingPoint() = default;

            /// @brief Binds a sampler to the binding point.
            ///
            /// If this binding point doesn't support a sampler, an error is logged.
            ///
            /// @param sampler Sampler to bind.
            virtual void bind(gl::Sampler sampler) = 0;

            /// @brief Binds a 1D texture to the binding point.
            ///
            /// If this binding point doesn't support a 1D texture, an error is logged.
            ///
            /// @param tex Texture to bind.
            virtual void bind(gl::Texture1D tex) = 0;

            /// @brief Binds a 2D texture to the binding point.
            ///
            /// If this binding point doesn't support a 2D texture, an error is logged.
            ///
            /// @param tex Texture to bind.
            virtual void bind(gl::Texture2D tex) = 0;

            /// @brief Binds a 2D texture array to the binding point.
            ///
            /// If this binding point doesn't support a 2D texture array, an error is logged.
            ///
            /// @param tex Texture array to bind.
            virtual void bind(gl::Texture2DArray tex) = 0;

            /// @brief Binds a 3D texture to the binding point.
            ///
            /// If this binding point doesn't support a 3D texture, an error is logged.
            ///
            /// @param tex Texture to bind.
            virtual void bind(gl::Texture3D tex) = 0;

            /// @brief Binds a cube map to the binding point.
            ///
            /// If this binding point doesn't support a cube map, an error is logged.
            ///
            /// @param cubeMap Cube map to bind.
            virtual void bind(gl::CubeMap cubeMap) = 0;

            /// @brief Binds a cube map array to the binding point.
            ///
            /// If this binding point doesn't support a cube map array, an error is logged.
            ///
            /// @param cubeMap Cube map to bind.
            virtual void bind(gl::CubeMapArray cubeMap) = 0;

            /// @brief Binds a constant buffer to the binding point.
            ///
            /// If this binding point doesn't support a constant buffer, an error is logged.
            ///
            /// @param cb Constant buffer to bind.
            virtual void bind(gl::ConstantBuffer cb) = 0;

            /// @brief Binds a level of a 2D texture to an image unit.
            ///
            /// If this binding point doesn't support an image unit, an error is logged.
            ///
            /// @note Unsupported on some platforms. Support can be queried using @ref
            /// gl::RenderDevice::getProperty with @ref gl::Property::ComputeSupported.
            /// @param tex Texture to bind.
            /// @param level Mip level to bind.
            /// @param access Access mode.
            virtual void bind(gl::Texture2D tex, int level, Access access) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided vec2
            /// value.
            /// @param val Value to set.
            virtual void setConstant(glm::vec2 val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided vec3
            /// value.
            /// @param val Value to set.
            virtual void setConstant(glm::vec3 val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided vec4
            /// value.
            /// @param val Value to set.
            virtual void setConstant(glm::vec4 val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided
            /// integer vec2 value.
            /// @param val Value to set.
            virtual void setConstant(glm::ivec2 val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided
            /// integer vec3 value.
            /// @param val Value to set.
            virtual void setConstant(glm::ivec3 val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided
            /// integer vec4 value.
            /// @param val Value to set.
            virtual void setConstant(glm::ivec4 val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided
            /// unsigned integer vec2 value.
            /// @param val Value to set.
            virtual void setConstant(glm::uvec2 val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided
            /// unsigned integer vec3 value.
            /// @param val Value to set.
            virtual void setConstant(glm::uvec3 val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided
            /// unsigned integer vec4 value.
            /// @param val Value to set.
            virtual void setConstant(glm::uvec4 val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided mat4
            /// value.
            /// @param val Value to set.
            virtual void setConstant(glm::mat4 val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided
            /// float value.
            /// @param val Value to set.
            virtual void setConstant(float val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided int
            /// value.
            /// @param val Value to set.
            virtual void setConstant(int val) = 0;

            /// @brief Sets the value of the uniform tied to the binding point to the provided
            /// unsigned int value.
            /// @param val Value to set.
            virtual void setConstant(unsigned int val) = 0;

            /// @brief Gets the constant buffer structure of this binding point.
            ///
            /// If this binding point doesn't support a constant buffer, an error is logged.
            ///
            /// @return Whether the query was successful.
            virtual bool queryConstantBufferStructure(ConstantBufferStructure* structure) = 0;

        protected:
            ShaderBindingPoint() = default;
        };
    } // namespace impl

    // Operator overloads for MemoryBarriers.

    inline MemoryBarriers operator|(MemoryBarriers lhs, MemoryBarriers rhs)
    {
        return static_cast<MemoryBarriers>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    inline MemoryBarriers operator&(MemoryBarriers lhs, MemoryBarriers rhs)
    {
        return static_cast<MemoryBarriers>(static_cast<int>(lhs) & static_cast<int>(rhs));
    }

    inline MemoryBarriers& operator|=(MemoryBarriers& lhs, MemoryBarriers rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

    inline MemoryBarriers& operator&=(MemoryBarriers& lhs, MemoryBarriers rhs)
    {
        lhs = lhs & rhs;
        return lhs;
    }

} // namespace cubos::core::gl
