/// @file
/// @brief Resource @ref cubos::engine::Gizmos.
/// @ingroup gizmos-plugin

#pragma once

#include <vector>

#include <glm/vec3.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/cubos.hpp>

namespace cubos::engine
{
    class GizmosRenderer;

    /// @brief Resource which queues commands for drawing gizmos, basic primitives useful for debugging and tools.
    ///
    /// @ingroup gizmos-plugin
    class Gizmos final
    {
    public:
        /// @brief Sets the color to be used when drawing any subsequent gizmos.
        /// @param color Color to be used.
        void color(const glm::vec3& color);

        /// @brief Draws a line gizmo.
        /// @param id Identifier of the gizmo.
        /// @param from One of the ends of the line to be drawn.
        /// @param to The other end of the line to be drawn.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        void drawLine(const std::string& id, glm::vec3 from, glm::vec3 to, float lifespan = 0.0F);

        /// @brief Draws a filled box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param corner One of the corners of the box to be drawn.
        /// @param oppositeCorner The opposite corner of the box to be drawn.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        void drawBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan = 0.0F);

        /// @brief Draws a wireframe box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param corner One of the corners of the box to be drawn.
        /// @param oppositeCorner The opposite corner of the box to be drawn.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        void drawWireBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan = 0.0F);

        /// @brief Class that describes a type of gizmo
        class Gizmo
        {
        public:
            virtual ~Gizmo() = default;

            Gizmo(const std::string& id, glm::vec3 color, float lifespan);

            /// @brief Draws the gizmo to screen.
            /// @param renderer Renderer.
            virtual void draw(GizmosRenderer& renderer) = 0;

            /// @brief Decreases the time the gizmo has left before it is destroyed.
            /// @param delta Seconds since the last frame.
            bool decreaseLifespan(float delta);

        protected:
            const std::string& mId; ///< Gizmo identifier
            glm::vec3 mColor;       ///< Color of the gizmo
            float mLifespan;        ///< Time in seconds the gizmo has left to live
        };

        std::vector<std::shared_ptr<Gizmo>> gizmos; ///< Queued gizmos to be drawn.

    private:
        glm::vec3 mColor; ///< Currently set color
    };

} // namespace cubos::engine
