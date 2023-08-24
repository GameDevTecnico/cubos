/// @file
/// @brief Resource @ref cubos::engine::RendererFrame.
/// @ingroup renderer-plugin

#pragma once

#include <vector>

#include <cubos/engine/renderer/renderer.hpp>

namespace cubos::engine
{
    /// @brief Resource which describes a scene to be drawn by the renderer.
    ///
    /// Each frame, the @ref renderer-plugin will query entities with renderer components, such as
    /// @ref RenderableGrid and @ref PointLight, and add them to the @ref RendererFrame.
    ///
    /// @ingroup renderer-plugin
    class RendererFrame final
    {
    public:
        RendererFrame() = default;
        ~RendererFrame() = default;

        /// @brief Data of a single draw command.
        struct DrawCmd
        {
            RendererGrid grid;  ///< Grid to be drawn.
            glm::mat4 modelMat; ///< Model transform matrix.
        };

        /// @brief Submits a draw command.
        /// @param grid Handle of the grid to draw.
        /// @param modelMat Model matrix of the grid, used for applying transformations.
        void draw(RendererGrid grid, glm::mat4 modelMat);

        /// @brief Sets the ambient light of the scene.
        /// @param color Color of the ambient light.
        void ambient(const glm::vec3& color);

        /// @brief Sets the sky gradient of the scene.
        /// @param bottom Bottom sky color.
        /// @param top Top sky color.
        void skyGradient(glm::vec3 bottom, glm::vec3 top);

        /// @brief Adds a spot light to the frame.
        /// @param light Spot light to add.
        void light(const core::gl::SpotLight& light);

        /// @brief Adds a directional light to the frame.
        /// @param light Directional light to add.
        void light(const core::gl::DirectionalLight& light);

        /// @brief Adds a point light to the frame.
        /// @param light Point light to add.
        void light(const core::gl::PointLight& light);

        /// @brief Clears the frame, removing all draw calls and lights.
        void clear();

        /// @brief Gets all of the draw commands stored in the frame.
        /// @return Draw commands.
        const std::vector<DrawCmd>& drawCmds() const;

        /// @brief Gets the ambient light of the scene.
        /// @return Dmbient light.
        const glm::vec3& ambient() const;

        /// @brief Gets the sky gradient of the scene.
        /// @param i Index of the color (0 = bottom, 1 = top).
        /// @return Ambient light.
        const glm::vec3& skyGradient(int i) const;

        /// @brief Gets the spot lights of the frame.
        /// @return Spot lights.
        const std::vector<core::gl::SpotLight>& spotLights() const;

        /// @brief Gets the directional lights of the frame.
        /// @return Directional lights.
        const std::vector<core::gl::DirectionalLight>& directionalLights() const;

        /// @brief Gets the point lights of the frame.
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
