#pragma once

#include <string>

#include <cubos/core/gl/render_device.hpp>

namespace cubos::engine
{
    /// @brief Resource that holds the information needed to draw a gizmo
    class GizmosRenderer final
    {
    public:
        static constexpr int CutConeVertsPerBase = 16; ///< Number of vertexes in each face of a cut cone gizmo.
        static constexpr int RingSections = 32;        ///< Number of traversal sections (i.e. quads) in a ring gizmo.

        cubos::core::gl::ShaderPipeline drawPipeline; ///< Shader pipeline to be used when drawing gizmos.
        cubos::core::gl::ShaderPipeline idPipeline;   ///< Shader pipeline to be used when drawing gizmos.
        cubos::core::gl::RenderDevice* renderDevice;  ///< Active render device.

        /// @brief Set of buffers and structs that hold te information needed to draw a specific type of mesh.
        struct Primitive
        {
            cubos::core::gl::VertexBuffer vb;
            cubos::core::gl::VertexArray va;
            cubos::core::gl::IndexBuffer ib;
            cubos::core::gl::VertexArrayDesc vaDesc;
        };

        Primitive linePrimitive;    ///< GL line information.
        Primitive boxPrimitive;     ///< GL box information.
        Primitive cutConePrimitive; ///< GL cut cone information.
        Primitive ringPrimitive;    ///< GL ring information.

        glm::ivec2 textureSize;               ///< Size of the id texture.
        cubos::core::gl::Texture2D idTexture; ///< Texture holding the ids of the gizmo that was drawn to each pixel.
        cubos::core::gl::Framebuffer idFramebuffer; ///< Buffer holding the id texture.

        cubos::core::gl::DepthStencilState doDepthCheckStencilState; ///< Stencil State that performs depth checks.
        cubos::core::gl::DepthStencilState noDepthCheckStencilState; ///< Stencil State that ignores depth checks.

        /// @brief Sets up the render device to be used.
        /// @param renderDevice the current Render device being used.
        void init(cubos::core::gl::RenderDevice* currentRenderDevice, glm::ivec2 size);

        /// @brief Resizes the id texture.
        /// @param size New size of the texture.
        void resizeTexture(glm::ivec2 size);

        glm::ivec2 lastMousePosition; ///< Cursor position.
        bool mousePressed;            ///< Whether or not the mouse left button is pressed.

    private:
        cubos::core::gl::Texture2D mDepthTexture;

        void initIdTexture(glm::ivec2 size);

        void initIdPipeline();
        void initDrawPipeline();

        void initLinePrimitive();
        void initBoxPrimitive();
        void initCutConePrimitive();
        void initRingPrimitive();
    };
} // namespace cubos::engine
