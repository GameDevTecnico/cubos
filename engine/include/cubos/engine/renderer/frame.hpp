#pragma once

#include <vector>

#include <cubos/engine/renderer/renderer.hpp>

namespace cubos::engine
{
    /// Contains all scene data needed to draw a frame.
    /// For each frame, the engine will generate a frame object with data fetched from the scene, and then, provide it
    /// to the renderer. The renderer is responsible for showing this data on the screen.
    class RendererFrame final
    {
    public:
        RendererFrame() = default;
        ~RendererFrame() = default;

        /// Data of a single draw command.
        struct DrawCmd
        {
            RendererGrid grid;  ///< Grid to be drawn.
            glm::mat4 modelMat; ///< Model transform matrix.
        };

        /// Submits a draw command.
        /// @param grid The handle of the grid to draw.
        /// @param modelMat The model matrix of the grid, used for applying transformations.
        void draw(RendererGrid grid, glm::mat4 modelMat);

        /// Sets the ambient light of the scene.
        /// @param color The color of the ambient light.
        void ambient(const glm::vec3& color);

        /// Sets the sky gradient of the scene.
        /// @param bottom Bottom sky color.
        /// @param top Top sky color.
        void skyGradient(glm::vec3 bottom, glm::vec3 top);

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
        const std::vector<DrawCmd>& drawCmds() const;

        /// Gets the ambient light of the scene.
        /// @return The ambient light.
        const glm::vec3& ambient() const;

        /// Gets the sky gradient of the scene.
        /// @param i Index of the color (0 = bottom, 1 = top).
        /// @return The ambient light.
        const glm::vec3& skyGradient(int i) const;

        /// Gets the spot lights of the frame.
        /// @return The spot lights.
        const std::vector<core::gl::SpotLight>& spotLights() const;

        /// Gets the directional lights of the frame.
        /// @return The directional lights.
        const std::vector<core::gl::DirectionalLight>& directionalLights() const;

        /// Gets the point lights of the frame.
        /// @return The point lights.
        const std::vector<core::gl::PointLight>& pointLights() const;

    private:
        glm::vec3 mAmbientColor;
        glm::vec3 mSkyGradient[2];
        std::vector<DrawCmd> mDrawCmds;
        std::vector<core::gl::SpotLight> mSpotLights;
        std::vector<core::gl::DirectionalLight> mDirectionalLights;
        std::vector<core::gl::PointLight> mPointLights;
    };
} // namespace cubos::engine
