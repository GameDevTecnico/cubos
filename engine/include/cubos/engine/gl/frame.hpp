#ifndef CUBOS_ENGINE_GL_FRAME_HPP
#define CUBOS_ENGINE_GL_FRAME_HPP

#include <cubos/engine/gl/renderer.hpp>

#include <vector>

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

        /// Data of a single draw command.
        struct DrawCmd
        {
            RendererGrid grid;
            glm::mat4 modelMat;
        };

        /// Submits a draw command.
        /// @param grid The handle of the grid to draw.
        /// @param modelMat The model matrix of the grid, used for applying transformations.
        void draw(RendererGrid grid, glm::mat4 modelMat);

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

        /// Gets all of the draw commands stored in the frame.
        /// @return The draw commands.
        const std::vector<DrawCmd>& getDrawCmds() const;

        /// Gets the ambient light of the scene.
        /// @return The ambient light.
        const glm::vec3& getAmbient() const;

        /// Gets the spot lights of the frame.
        /// @return The spot lights.
        const std::vector<core::gl::SpotLight>& getSpotLights() const;

        /// Gets the directional lights of the frame.
        /// @return The directional lights.
        const std::vector<core::gl::DirectionalLight>& getDirectionalLights() const;

        /// Gets the point lights of the frame.
        /// @return The point lights.
        const std::vector<core::gl::PointLight>& getPointLights() const;

    private:
        glm::vec3 ambientColor;
        std::vector<DrawCmd> drawCmds;
        std::vector<core::gl::SpotLight> spotLights;
        std::vector<core::gl::DirectionalLight> directionalLights;
        std::vector<core::gl::PointLight> pointLights;
    };
} // namespace cubos::engine::gl

#endif // CUBOS_ENGINE_GL_FRAME_HPP
