#ifndef CUBOS_CORE_GL_RENDER_DEVICE_HPP
#define CUBOS_CORE_GL_RENDER_DEVICE_HPP

#include <memory>
#include <variant>

#include <glm/glm.hpp>

#define CUBOS_CORE_GL_MAX_FRAMEBUFFER_RENDER_TARGET_COUNT 8
#define CUBOS_CORE_GL_MAX_TEXTURE_2D_ARRAY_SIZE 256
#define CUBOS_CORE_GL_MAX_CUBEMAP_ARRAY_SIZE 256
#define CUBOS_CORE_GL_MAX_MIP_LEVEL_COUNT 8
#define CUBOS_CORE_GL_MAX_CONSTANT_BUFFER_ELEMENT_NAME_SIZE 32
#define CUBOS_CORE_GL_MAX_CONSTANT_BUFFER_ELEMENT_COUNT 32
#define CUBOS_CORE_GL_MAX_VERTEX_ARRAY_ELEMENT_COUNT 8
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

    using Framebuffer = std::shared_ptr<impl::Framebuffer>;
    using RasterState = std::shared_ptr<impl::RasterState>;
    using DepthStencilState = std::shared_ptr<impl::DepthStencilState>;
    using BlendState = std::shared_ptr<impl::BlendState>;

    using Sampler = std::shared_ptr<impl::Sampler>;
    using Texture1D = std::shared_ptr<impl::Texture1D>;
    using Texture2D = std::shared_ptr<impl::Texture2D>;
    using Texture2DArray = std::shared_ptr<impl::Texture2DArray>;
    using Texture3D = std::shared_ptr<impl::Texture3D>;
    using CubeMap = std::shared_ptr<impl::CubeMap>;
    using CubeMapArray = std::shared_ptr<impl::CubeMapArray>;

    using ConstantBuffer = std::shared_ptr<impl::ConstantBuffer>;
    using IndexBuffer = std::shared_ptr<impl::IndexBuffer>;
    using VertexBuffer = std::shared_ptr<impl::VertexBuffer>;
    using VertexArray = std::shared_ptr<impl::VertexArray>;

    using ShaderStage = std::shared_ptr<impl::ShaderStage>;
    using ShaderPipeline = std::shared_ptr<impl::ShaderPipeline>;
    using ShaderBindingPoint = impl::ShaderBindingPoint*;

    /// Render device properties that can be queried at runtime.
    /// @see RenderDevice::getProperty().
    enum class Property
    {
        MaxAnisotropy,    ///< Specifies the upper bound of anisotropic filtering.
        ComputeSupported, ///< Specifies whether compute shaders are supported (0 or 1).
    };

    /// Usage mode for buffers and textures.
    enum class Usage
    {
        Default,
        Dynamic,
        Static,
    };

    /// Data type.
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

    /// Index format.
    enum class IndexFormat
    {
        UShort, // 16 bits unsigned integer.
        UInt,   // 32 bits unsigned integer.
    };

    /// Texture format.
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

    /// Texture address mode.
    enum class AddressMode
    {
        Repeat,
        Mirror,
        Clamp,
        Border,
    };

    /// Texture filter type.
    enum class TextureFilter
    {
        None,
        Nearest,
        Linear,
    };

    /// Triangle winding mode.
    enum class Winding
    {
        CW, ///< Clockwise.
        CCW ///< Counterclockwise.
    };

    /// Specifies a face.
    enum class Face
    {
        Front,
        Back,
        FrontAndBack,
    };

    /// Rasterizer mode.
    enum class RasterMode
    {
        Wireframe,
        Fill,
    };

    /// Comparison function.
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

    /// Stencil action.
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

    /// Blend factor.
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

    /// Blend operation.
    enum class BlendOp
    {
        Add,
        Substract,
        RevSubstract,
        Min,
        Max,
    };

    /// Shader stage type.
    enum class Stage
    {
        Vertex,
        Geometry,
        Pixel,
        Compute,
    };

    /// Cube map face.
    enum class CubeFace
    {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5,
    };

    /// Memory barrier flags for synchronization.
    enum class MemoryBarriers
    {
        /// Utility flag to indicate that no memory barrier is set.
        None = 0,

        /// If set, data sourced from vertex buffers after the barrier will reflect the data written
        /// by shaders prior to the barrier.
        VertexBuffer = 1,

        /// If set, data sourced from index buffers after the barrier will reflect the data written
        /// by shaders prior to the barrier.
        IndexBuffer = 2,

        /// If set, data sourced from constant buffers after the barrier will reflect the data written
        /// by shaders prior to the barrier.
        ConstantBuffer = 4,

        /// If set, memory accesses using shader image load, store, and atomic built-in functions issued after the
        /// barrier will reflect the data written by shaders prior to the barrier.
        ImageAccess = 8,

        /// If set, texture accesses from shaders after the barrier will reflect the data written by shaders
        /// prior to the barrier.
        TextureAccess = 16,

        /// If set, reads and writes via framebuffer objects after the barrier will reflect the data written
        /// by shaders prior to the barrier.
        Framebuffer = 32,

        /// Utility flag to set all memory barriers.
        All = VertexBuffer | IndexBuffer | ConstantBuffer | ImageAccess | TextureAccess | Framebuffer,
    };

    /// Access mode for a resource.
    enum class Access
    {
        Read,      ///< Read access.
        Write,     ///< Write access.
        ReadWrite, ///< Read and write access.
    };

    enum class BufferStorageType
    {
        Small,
        Large,
    };

    /// Framebuffer description.
    struct FramebufferDesc
    {
        struct CubeMapTarget
        {
            CubeMap handle; ///< Cube map handle.
            CubeFace face;  ///< Cube map face which will be used as target.
        };

        struct Texture2DTarget
        {
            Texture2D handle; ///< Texture handle.
        };

        struct CubeMapArrayTarget
        {
            CubeMapArray handle; ///< Cube map array handle.
        };

        struct Texture2DArrayTarget
        {
            Texture2DArray handle; ///< Texture array handle.
        };

        enum class TargetType
        {
            CubeMap,
            Texture2D,
            CubeMapArray,
            Texture2DArray
        };

        struct FramebufferTarget
        {
        public:
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
            bool set = false;
            TargetType type; ///< Type of the currently set target.

            std::variant<CubeMapTarget, Texture2DTarget, CubeMapArrayTarget, Texture2DArrayTarget> target;
        } targets[CUBOS_CORE_GL_MAX_FRAMEBUFFER_RENDER_TARGET_COUNT]; ///< Render targets.

        uint32_t targetCount = 1;       ///< Number of render targets.
        FramebufferTarget depthStencil; ///< Optional depth stencil target.
    };

    /// Rasterizer state description.
    struct RasterStateDesc
    {
        bool cullEnabled = false;                 ///< Is face culling enabled?
        Face cullFace = Face::Back;               ///< Face that will be culled.
        Winding frontFace = Winding::CCW;         ///< Front face winding.
        RasterMode rasterMode = RasterMode::Fill; ///< Rasterizer mode.
        bool scissorEnabled = false;              ///< Is scissor test enabled?
    };

    /// Depth stencil state description.
    struct DepthStencilStateDesc
    {
        /// Depth state description.
        struct Depth
        {
            bool enabled = false;            ///< Enable depth checks?
            bool writeEnabled = false;       ///< Write to depth?
            float near = 0.0f;               ///< Depth near value.
            float far = 1.0f;                ///< Depth far value.
            Compare compare = Compare::Less; ///< Depth comparison function.
        };

        /// Stencil state description.
        struct Stencil
        {
            /// Stencil face description.
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

    /// Blend state description.
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

    /// Sampler description.
    struct SamplerDesc
    {
        float borderColor[4] = {0.0f}; ///< Border color applied when the address mode is AddressMode::Border.

        TextureFilter minFilter = TextureFilter::Nearest; ///< Minifying filter.
        TextureFilter magFilter = TextureFilter::Nearest; ///< Magnifying filter.
        TextureFilter mipmapFilter = TextureFilter::None; ///< Set to None to disable mipmapping.

        AddressMode addressU = AddressMode::Clamp; ///< Texture adress mode on coordinate U.
        AddressMode addressV = AddressMode::Clamp; ///< Texture adress mode on coordinate V.
        AddressMode addressW = AddressMode::Clamp; ///< Texture adress mode on coordinate W.

        std::size_t maxAnisotropy = 1; ///< Max anisotropy for filtering. Limited to Property::MaxAnisotropy.
    };

    /// 1D texture description.
    struct Texture1DDesc
    {
        const void* data[CUBOS_CORE_GL_MAX_MIP_LEVEL_COUNT] = {}; ///< Optional initial texture data.
        std::size_t mipLevelCount = 1;                            ///< Number of mip levels.
        std::size_t width;                                        ///< Texture width.
        Usage usage;                                              ///< Texture usage mode.
        TextureFormat format;                                     ///< Texture format.
    };

    /// 2D texture description.
    struct Texture2DDesc
    {
        const void* data[CUBOS_CORE_GL_MAX_MIP_LEVEL_COUNT] = {}; ///< Optional initial texture data.
        std::size_t mipLevelCount = 1;                            ///< Number of mip levels.
        std::size_t width;                                        ///< Texture width.
        std::size_t height;                                       ///< Texture height.
        Usage usage;                                              ///< Texture usage mode.
        TextureFormat format;                                     ///< Texture format.
    };

    /// 2D texture array description.
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

    /// 3D texture description.
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

    /// Cube map description.
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

    /// Cube map array description.
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

    /// Constant buffer element.
    struct ConstantBufferElement
    {
        char name[CUBOS_CORE_GL_MAX_CONSTANT_BUFFER_ELEMENT_NAME_SIZE]; ///< Element name.

        std::size_t offset; ///< Offset of the element in the buffer.
        std::size_t size;   ///< Number of values in the element if it is an array. If it isn't, this is set to 1.
        std::size_t stride; ///< Stride between each element in the array, if the element is an array.
    };

    /// Constant buffer structure.
    struct ConstantBufferStructure
    {
        std::size_t size;         ///< Size of the buffer in bytes.
        std::size_t elementCount; ///< Number of elements.
        ConstantBufferElement elements[CUBOS_CORE_GL_MAX_CONSTANT_BUFFER_ELEMENT_COUNT]; ///< Constant buffer elements.
    };

    /// Vertex element description.
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

    /// Vertex array description.
    struct VertexArrayDesc
    {
        std::size_t elementCount = 0;                                         ///< Number of vertex elements.
        VertexElement elements[CUBOS_CORE_GL_MAX_VERTEX_ARRAY_ELEMENT_COUNT]; ///< Vertex elements.
        VertexBuffer buffers[CUBOS_CORE_GL_MAX_VERTEX_ARRAY_BUFFER_COUNT];    ///< Vertex buffers.
        ShaderPipeline shaderPipeline; ///< Shader pipeline used with the vertex array.
    };

    /// Abstract render device type, used to wrap low-level rendering APIs such as OpenGL.
    /// @see io::Window, gl::OGLRenderDevice.
    class RenderDevice
    {
    public:
        RenderDevice() = default;
        virtual ~RenderDevice() = default;
        RenderDevice(const RenderDevice&) = delete;

        /// Creates a new framebuffer.
        /// @return Framebuffer handle, or nullptr if the creation failed.
        virtual Framebuffer createFramebuffer(const FramebufferDesc& desc) = 0;

        /// Sets the current framebuffer.
        virtual void setFramebuffer(Framebuffer fb) = 0;

        /// Creates a new rasterizer state.
        /// @return Rasterizer state handle, or nullptr if the creation failed.
        virtual RasterState createRasterState(const RasterStateDesc& desc) = 0;

        /// Sets the current rasterizer state.
        virtual void setRasterState(RasterState rs) = 0;

        /// Creates a new depth stencil state.
        /// @return Depth stencil state handle, or nullptr if the creation failed.
        virtual DepthStencilState createDepthStencilState(const DepthStencilStateDesc& desc) = 0;

        /// Sets the current depth stencil state.
        virtual void setDepthStencilState(DepthStencilState dss) = 0;

        /// Creates a new blend state.
        /// @return Blend state handle, or nullptr if the creation failed.
        virtual BlendState createBlendState(const BlendStateDesc& desc) = 0;

        /// Sets the current blend state.
        virtual void setBlendState(BlendState bs) = 0;

        /// Creates a new texture sampler.
        /// @return Sampler handle, or nullptr if the creation failed.
        virtual Sampler createSampler(const SamplerDesc& desc) = 0;

        /// Creates a new 1D texture.
        /// @return Texture handle, or nullptr if the creation failed.
        virtual Texture1D createTexture1D(const Texture1DDesc& desc) = 0;

        /// Creates a new 2D texture.
        /// @return Texture handle, or nullptr if the creation failed.
        virtual Texture2D createTexture2D(const Texture2DDesc& desc) = 0;

        /// Creates a new 2D texture array.
        /// @return Texture array handle, or nullptr if the creation failed.
        virtual Texture2DArray createTexture2DArray(const Texture2DArrayDesc& desc) = 0;

        /// Creates a new 3D texture.
        /// @return Texture handle, or nullptr if the creation failed.
        virtual Texture3D createTexture3D(const Texture3DDesc& desc) = 0;

        /// Creates a new cube map.
        /// @return Cube map handle, or nullptr if the creation failed.
        virtual CubeMap createCubeMap(const CubeMapDesc& desc) = 0;

        /// Creates a new cube map array.
        /// @return Cube map array handle, or nullptr if the creation failed.
        virtual CubeMapArray createCubeMapArray(const CubeMapArrayDesc& desc) = 0;

        /// Creates a new constant buffer.
        /// @param size Size in bytes.
        /// @param data Initial data, can be nullptr.
        /// @param usage The usage which the buffer will have.
        /// @return Constant buffer handle, or nullptr if the creation failed.
        virtual ConstantBuffer createConstantBuffer(std::size_t size, const void* data, Usage usage) = 0;

        /// Creates a new index buffer.
        /// @param size Size in bytes.
        /// @param data Initial data, can be nullptr.
        /// @param format Index format.
        /// @param usage The usage which the buffer will have.
        /// @return Index buffer handle, or nullptr if the creation failed.
        virtual IndexBuffer createIndexBuffer(std::size_t size, const void* data, IndexFormat format, Usage usage) = 0;

        /// Sets the current index buffer.
        virtual void setIndexBuffer(IndexBuffer ib) = 0;

        /// Creates a new vertex buffer.
        /// @param size Size in bytes.
        /// @param data Initial data, can be nullptr.
        /// @param usage The usage which the buffer will have.
        /// @return Vertex buffer handle, or nullptr if the creation failed.
        virtual VertexBuffer createVertexBuffer(std::size_t size, const void* data, Usage usage) = 0;

        /// Creates a new vertex array.
        /// @return Vertex array handle, or nullptr if the creation failed.
        virtual VertexArray createVertexArray(const VertexArrayDesc& desc) = 0;

        /// Sets the current vertex array.
        virtual void setVertexArray(VertexArray va) = 0;

        /// Creates a new shader stage from GLSL source code.
        /// Compute shaders aren't supported on some platforms. Support can be queried using
        /// `renderDevice.getProperty(Property::ComputeSupported)`.
        /// @return Shader stage handle, or nullptr if the creation failed.
        virtual ShaderStage createShaderStage(Stage stage, const char* src) = 0;

        /// Creates a new shader pipeline from a vertex and pixel shaders.
        /// @param vs Vertex shader stage.
        /// @param ps Pixel shader stage.
        /// @return Shader pipeline handle, or nullptr if the creation failed.
        virtual ShaderPipeline createShaderPipeline(ShaderStage vs, ShaderStage ps) = 0;

        /// Creates a new shader pipeline from a vertex and pixel shaders.
        /// @param vs Vertex shader stage.
        /// @param gs Geometry shader stage.
        /// @param ps Pixel shader stage.
        /// @return Shader pipeline handle, or nullptr if the creation failed.
        virtual ShaderPipeline createShaderPipeline(ShaderStage vs, ShaderStage gs, ShaderStage ps) = 0;

        /// Creates a new shader pipeline from a compute shader.
        /// Compute shaders aren't supported on some platforms. Support can be queried using
        /// `renderDevice.getProperty(Property::ComputeSupported)`.
        /// @param cs Compute shader stage.
        /// @return Shader pipeline handle, or nullptr if the creation failed.
        virtual ShaderPipeline createShaderPipeline(ShaderStage cs) = 0;

        /// Sets the current shader pipeline used for rendering.
        virtual void setShaderPipeline(ShaderPipeline pipeline) = 0;

        /// Clears the color buffer bit on the current framebuffer to a specific color.
        virtual void clearColor(float r, float g, float b, float a) = 0;

        /// Clears the color buffer of a specific target on the current framebuffer to a specific color.
        virtual void clearTargetColor(std::size_t target, float r, float g, float b, float a) = 0;

        /// Clears the depth buffer bit on the current framebuffer to a specific value.
        virtual void clearDepth(float depth) = 0;

        /// Clears the stencil buffer bit on the current framebuffer to a specific value.
        virtual void clearStencil(int stencil) = 0;

        /// Draws tringles.
        /// @param offset Index of the first vertex to be drawn.
        /// @param count Number of vertices that will be drawn.
        virtual void drawTriangles(std::size_t offset, std::size_t count) = 0;

        /// Draws tringles with an index buffer.
        /// @param offset Index of the first indice to be drawn.
        /// @param count Number of indices that will be drawn.
        virtual void drawTrianglesIndexed(std::size_t offset, std::size_t count) = 0;

        /// Draws tringles multiple times.
        /// @param offset Index of the first vertex to be drawn.
        /// @param count Number of vertices that will be drawn.
        /// @param instanceCount Number of instances drawn.
        virtual void drawTrianglesInstanced(std::size_t offset, std::size_t count, std::size_t instanceCount) = 0;

        /// Draws tringles multiple times with an index buffer.
        /// @param offset Index of the first indice to be drawn.
        /// @param count Number of indices that will be drawn.
        /// @param instanceCount Number of instances drawn.
        virtual void drawTrianglesIndexedInstanced(std::size_t offset, std::size_t count,
                                                   std::size_t instanceCount) = 0;

        /// Dispatches a compute pipeline.
        /// @param x X dimension of the work group.
        /// @param y Y dimension of the work group.
        /// @param z Z dimension of the work group.
        virtual void dispatchCompute(std::size_t x, std::size_t y, std::size_t z) = 0;

        /// Defines a barrier ordering memory transactions.
        /// This ensure that all memory transactions before the barrier are completed before the
        /// barrier is executed.
        /// This function isn't supported on some platforms. Support can be queried using
        /// `renderDevice.getProperty(Property::ComputeSupported)`.
        /// @param barriers The barriers to apply.
        virtual void memoryBarrier(MemoryBarriers barriers) = 0;

        /// Sets the current viewport.
        /// @param x Bottom left viewport corner X coordinate.
        /// @param y Bottom left viewport corner Y coordinate.
        /// @param w Viewport width.
        /// @param h Viewport height.
        virtual void setViewport(int x, int y, int w, int h) = 0;

        /// Sets the current scissor rectangle.
        /// @param x Bottom left scissor rectangle corner X coordinate.
        /// @param y Bottom left scissor rectangle corner Y coordinate.
        /// @param w Scissor rectangle width.
        /// @param h Scissor rectangle height.
        virtual void setScissor(int x, int y, int w, int h) = 0;

        /// Gets a runtime property of the render device.
        /// @param prop Property name.
        virtual int getProperty(Property prop) = 0;
    };

    /// Abstract gl types are defined inside this namespace, they should be used (derived) only in render device
    /// implementations.
    namespace impl
    {
        /// Abstract framebuffer, should not be used directly.
        class Framebuffer
        {
        protected:
            Framebuffer() = default;
            virtual ~Framebuffer() = default;
        };

        /// Abstract rasterizer state, should not be used directly.
        class RasterState
        {
        protected:
            RasterState() = default;
            virtual ~RasterState() = default;
        };

        /// Abstract depth stencil state, should not be used directly.
        class DepthStencilState
        {
        protected:
            DepthStencilState() = default;
            virtual ~DepthStencilState() = default;
        };

        /// Abstract blend state, should not be used directly.
        class BlendState
        {
        protected:
            BlendState() = default;
            virtual ~BlendState() = default;
        };

        /// Abstract sampler, should not be used directly.
        class Sampler
        {
        protected:
            Sampler() = default;
            virtual ~Sampler() = default;
        };

        /// Abstract 1D texture, should not be used directly.
        class Texture1D
        {
        public:
            /// Updates the texture with new data, which must have the same format used when the texture was created.
            /// @param x Destination X coordinate.
            /// @param width Width of the section which will be updated.
            /// @param data Pointer to the new data.
            /// @param level Mip level to update.
            virtual void update(std::size_t x, std::size_t width, const void* data, std::size_t level = 0) = 0;

            /// Generates mipmaps on this texture.
            virtual void generateMipmaps() = 0;

        protected:
            Texture1D() = default;
            virtual ~Texture1D() = default;
        };

        /// Abstract 2D texture, should not be used directly.
        class Texture2D
        {
        public:
            /// Updates the texture with new data, which must have the same format used when the texture was created.
            /// @param x Destination X coordinate.
            /// @param y Destination Y coordinate.
            /// @param width Width of the section which will be updated.
            /// @param height Height of the section which will be updated.
            /// @param data Pointer to the new data.
            /// @param level Mip level to update.
            virtual void update(std::size_t x, std::size_t y, std::size_t width, std::size_t height, const void* data,
                                std::size_t level = 0) = 0;

            /// Generates mipmaps on this texture.
            virtual void generateMipmaps() = 0;

        protected:
            Texture2D() = default;
            virtual ~Texture2D() = default;
        };

        /// Abstract 2D texture array, should not be used directly.
        class Texture2DArray
        {
        public:
            /// Updates the texture with new data, which must have the same format used when the texture was created.
            /// @param x Destination X coordinate.
            /// @param y Destination Y coordinate.
            /// @param i Index of the destination texture within the array.
            /// @param width Width of the section which will be updated.
            /// @param height Height of the section which will be updated.
            /// @param data Pointer to the new data.
            /// @param level Mip level to update.
            virtual void update(std::size_t x, std::size_t y, std::size_t i, std::size_t width, std::size_t height,
                                const void* data, std::size_t level = 0) = 0;

            /// Generates mipmaps on this texture.
            virtual void generateMipmaps() = 0;

        protected:
            Texture2DArray() = default;
            virtual ~Texture2DArray() = default;
        };

        /// Abstract 3D texture, should not be used directly.
        class Texture3D
        {
        public:
            /// Updates the texture with new data, which must have the same format used when the texture was created.
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

            /// Generates mipmaps on this texture.
            virtual void generateMipmaps() = 0;

        protected:
            Texture3D() = default;
            virtual ~Texture3D() = default;
        };

        /// Abstract cube map, should not be used directly.
        class CubeMap
        {
        public:
            /// Updates a cube map's face with new data, which must have the same format used when the cube map was
            /// created.
            /// @param x Destination X coordinate.
            /// @param y Destination Y coordinate.
            /// @param width Width of the section which will be updated.
            /// @param height Height of the section which will be updated.
            /// @param data Pointer to the new data.
            /// @param face Face to update.
            /// @param level Mip level to update.
            virtual void update(std::size_t x, std::size_t y, std::size_t width, std::size_t height, const void* data,
                                CubeFace face, std::size_t level = 0) = 0;

            /// Generates mipmaps on this cube map.
            virtual void generateMipmaps() = 0;

        protected:
            CubeMap() = default;
            virtual ~CubeMap() = default;
        };

        /// Abstract cube map, should not be used directly.
        class CubeMapArray
        {
        public:
            /// Updates a cube map's face with new data, which must have the same format used when the cube map was
            /// created.
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

            /// Generates mipmaps on this cube map.
            virtual void generateMipmaps() = 0;

        protected:
            CubeMapArray() = default;
            virtual ~CubeMapArray() = default;
        };

        /// Abstract constant buffer, should not be used directly.
        class ConstantBuffer
        {
        public:
            /// Maps the constant buffer to a region in memory.
            /// @return Pointer to the memory region.
            virtual void* map() = 0;

            /// Unmaps the constant buffer, updating it with data written to the mapped region.
            virtual void unmap() = 0;

        protected:
            ConstantBuffer() = default;
            virtual ~ConstantBuffer() = default;
        };

        /// Abstract index buffer, should not be used directly.
        class IndexBuffer
        {
        public:
            /// Maps the index buffer to a region in memory.
            /// @return Pointer to the memory region.
            virtual void* map() = 0;

            /// Unmaps the index buffer, updating it with data written to the mapped region.
            virtual void unmap() = 0;

        protected:
            IndexBuffer() = default;
            virtual ~IndexBuffer() = default;
        };

        /// Abstract vertex buffer, should not be used directly.
        class VertexBuffer
        {
        public:
            /// Maps the vertex buffer to a region in memory.
            /// @return Pointer to the memory region.
            virtual void* map() = 0;

            /// Unmaps the vertex buffer, updating it with data written to the mapped region.
            virtual void unmap() = 0;

        protected:
            VertexBuffer() = default;
            virtual ~VertexBuffer() = default;
        };

        /// Abstract vertex array, should not be used directly.
        class VertexArray
        {
        protected:
            VertexArray() = default;
            virtual ~VertexArray() = default;
        };

        /// Abstract shader stage, should not be used directly.
        class ShaderStage
        {
        public:
            /// Gets the shader stage type.
            /// @see Stage.
            virtual Stage getType() = 0;

        protected:
            ShaderStage() = default;
            virtual ~ShaderStage() = default;
        };

        /// Abstract shader pipeline, should not be used directly.
        class ShaderPipeline
        {
        public:
            /// Gets a binding point from its name.
            /// @return Returns the binding point, or nullptr if no binding point is found.
            /// @see ShaderBindingPoint.
            virtual gl::ShaderBindingPoint getBindingPoint(const char* name) = 0;

        protected:
            ShaderPipeline() = default;
            virtual ~ShaderPipeline() = default;
        };

        /// Abstract shader binding point, should not be used directly.
        class ShaderBindingPoint
        {
        public:
            /// Binds a sampler to the binding point.
            /// If this binding point doesn't support a sampler, an error is logged and nothing is done.
            virtual void bind(gl::Sampler sampler) = 0;

            /// Binds a 1D texture to the binding point.
            /// If this binding point doesn't support a 1D texture, an error is logged and nothing is done.
            virtual void bind(gl::Texture1D tex) = 0;

            /// Binds a 2D texture to the binding point.
            /// If this binding point doesn't support a 2D texture, an error is logged and nothing is done.
            virtual void bind(gl::Texture2D tex) = 0;

            /// Binds a 2D texture array to the binding point.
            /// If this binding point doesn't support a 2D texture array, an error is logged and nothing is done.
            virtual void bind(gl::Texture2DArray tex) = 0;

            /// Binds a 3D texture to the binding point.
            /// If this binding point doesn't support a 3D texture, an error is logged and nothing is done.
            virtual void bind(gl::Texture3D tex) = 0;

            /// Binds a cube map to the binding point.
            /// If this binding point doesn't support a cube map, an error is logged and nothing is done.
            virtual void bind(gl::CubeMap cubeMap) = 0;

            /// Binds a cube map array to the binding point.
            /// If this binding point doesn't support a cube map array, an error is logged and nothing is done.
            virtual void bind(gl::CubeMapArray cubeMap) = 0;

            /// Binds a constant buffer to the binding point.
            /// If this binding point doesn't support a constant buffer, an error is logged and nothing is done.
            virtual void bind(gl::ConstantBuffer cb) = 0;

            /// Binds a level of a 2D texture to an image unit.
            /// If this binding point doesn't support an image unit, an error is logged and nothing is done.
            /// This function isn't supported on some platforms. Support can be queried using
            /// `renderDevice.getProperty(Property::ComputeSupported)`.
            virtual void bind(gl::Texture2D tex, int level, Access access) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided vec2 value.
            virtual void setConstant(glm::vec2 val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided vec3 value.
            virtual void setConstant(glm::vec3 val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided vec4 value.
            virtual void setConstant(glm::vec4 val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided integer vec2 value.
            virtual void setConstant(glm::ivec2 val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided integer vec3 value.
            virtual void setConstant(glm::ivec3 val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided integer vec4 value.
            virtual void setConstant(glm::ivec4 val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided unsigned integer vec2 value.
            virtual void setConstant(glm::uvec2 val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided unsigned integer vec3 value.
            virtual void setConstant(glm::uvec3 val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided unsigned integer vec4 value.
            virtual void setConstant(glm::uvec4 val) = 0;

            virtual void setConstant(glm::mat4 val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided float value.
            virtual void setConstant(float val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided int value.
            virtual void setConstant(int val) = 0;

            /// Sets the value of the uniform tied to the binding point to the provided unsigned int value.
            virtual void setConstant(unsigned int val) = 0;

            /// Gets the constant buffer structure of this binding point.
            /// If this binding point doesn't support a constant buffer, an error is logged and nothing is done.
            /// @return False if the query failed, otherwise true.
            virtual bool queryConstantBufferStructure(ConstantBufferStructure* structure) = 0;

        protected:
            ShaderBindingPoint() = default;
            virtual ~ShaderBindingPoint() = default;
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

#endif // CUBOS_CORE_GL_RENDER_DEVICE_HPP
