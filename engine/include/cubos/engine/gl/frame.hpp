#ifndef CUBOS_ENGINE_GL_FRAME_HPP
#define CUBOS_ENGINE_GL_FRAME_HPP

#include <cubos/engine/gl/renderer.hpp>

namespace cubos::engine::gl
{
    /// Contains all scene data needed to draw a frame.
    /// For each frame, the engine will generate a frame object with data fetched from the scene, and then, provide it
    /// to the renderer. The renderer is responsible for showing this data on the screen.
    class Frame final
    {
    public:
        Frame() = default;
        ~Frame() = default;

        /// Submits a draw call.
        /// @param grid The renderer identifier of the grid to draw.
        /// @param modelMat The model matrix of the grid, used for applying transformations.
        void draw(Renderer::GridID grid, glm::mat4 modelMat);

        /// Sets the ambient light of the scene.
        /// @param color The color of the ambient light.
        void ambient(const glm::vec3& color);

        /// Adds a spot light to the frame.
        /// @param light The spot light to add.
        void light(const core::gl::SpotLight& light);

        /// Adds a directional light to the frame.
        /// @param light The directional light to add.
        void light(const core::gl::DirectionalLight& light);

        /// Adds a point light to the frame.
        /// @param light The point light to add.
        void light(const core::gl::PointLight& light);

        /// Clears the frame, removing all draw calls and lights.
        void clear();

    private:
        // TODO: how to store this data and make it accessible to the renderer?
    };
} // namespace cubos::engine::gl

#endif // CUBOS_ENGINE_GL_FRAME_HPP
