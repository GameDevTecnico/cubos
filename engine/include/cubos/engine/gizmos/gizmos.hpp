/// @file
/// @brief Resource @ref cubos::engine::Gizmos.
/// @ingroup gizmos-plugin

#pragma once
#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/cubos.hpp>

using cubos::core::gl::RenderDevice;
using cubos::core::gl::ShaderPipeline;
using cubos::engine::DeltaTime;

namespace cubos::engine
{
    /// @brief Resource which draws gizmos.
    ///
    /// @ingroup gizmos-plugin
    class Gizmos final
    {
    public:
        /// @brief Initiates the Gizmos resource.
        /// @param renderDevice Active render device.
        void init(RenderDevice& renderDevice);

        /// @brief Sets the color to be used when drawing any subsequent gizmos.
        /// @param color Color to be used.
        void color(const glm::vec3& color);

        /// @brief Draws a line gizmo.
        /// @param id Identifier of the gizmo.
        /// @param from One of the ends of the line to be drawn.
        /// @param to The other end of the line to be drawn.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        void drawLine(const std::string& id, glm::vec3 from, glm::vec3 to, float lifespan = 0);

        /// @brief Draws a filled box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param corner One of the corners of the box to be drawn.
        /// @param oppositeCorner The opposite corner of the box to be drawn.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        void drawBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan = 0);

        /// @brief Draws a wireframe box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param corner One of the corners of the box to be drawn.
        /// @param oppositeCorner The opposite corner of the box to be drawn.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        void drawWireBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan = 0);

        /// @brief Draws all the gizmos that were called to be drawn.
        /// @param deltaTime Resource holding the time since the previous frame.
        void drawQueuedGizmos(DeltaTime deltaTime);

    private:
        ShaderPipeline mPipeline;    ///< Shader pipeline to be used when drawing gizmos
        RenderDevice* mRenderDevice; ///< Active render device
        glm::vec3 mColor;            ///< Currently set color
        class GizmoBase;
        class LineGizmo;
        class BoxGizmo;
        std::vector<std::shared_ptr<GizmoBase>> mGizmosVector; ///< Queued gizmos to be drawn.
    };

} // namespace cubos::engine
