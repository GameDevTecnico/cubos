#pragma once

#include <cubos/core/gl/render_device.hpp>

namespace cubos::engine
{
    /// @brief Resource that holds the information needed to draw a gizmo
    class GizmosRenderer final
    {
    public:
        cubos::core::gl::ShaderPipeline pipeline;    ///< Shader pipeline to be used when drawing gizmos
        cubos::core::gl::RenderDevice* renderDevice; ///< Active render device

        /// @brief Set of buffers and structs that hold te information needed to draw a specific type of mesh
        struct Primitive
        {
            cubos::core::gl::VertexBuffer vb;
            cubos::core::gl::VertexArray va;
            cubos::core::gl::IndexBuffer ib;
            cubos::core::gl::VertexArrayDesc vaDesc;
        };

        Primitive linePrimitive; ///< GL line information
        Primitive boxPrimitive;  ///< GL box information

        /// @brief Sets up the GizmosRenderer to be used
        /// @param renderDevice the current RenderDevice being used
        void init(cubos::core::gl::RenderDevice* currentRenderDevice);

    private:
        void initLinePrimitive();
        void initBoxPrimitive();
    };
} // namespace cubos::engine
