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
        /// @brief Space to draw a gizmo in.
        enum Space
        {
            World, ///< Draw gizmo in the game world.
            View,  ///< Draw gizmo in every camera's viewport.
            Screen ///< Draw gizmo in the screen.
        };

        /// @brief Sets the color to be used when drawing any subsequent gizmos.
        /// @param color Color to be used.
        void color(const glm::vec3& color);

        /// @brief Draws a line gizmo.
        /// @param id Identifier of the gizmo.
        /// @param from One of the ends of the line to be drawn.
        /// @param to The other end of the line to be drawn.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawLine(const std::string& id, glm::vec3 from, glm::vec3 to, float lifespan = 0.0F,
                      Space space = Space::World);

        /// @brief Draws a filled box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param corner One of the corners of the box to be drawn.
        /// @param oppositeCorner The opposite corner of the box to be drawn.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan = 0.0F,
                     Space space = Space::World);

        /// @brief Draws a cut cone gizmo.
        /// @param id Identifier of the gizmo.
        /// @param firstBaseCenter Center of one of the bases.
        /// @param firstBaseRadius Radius of one of the bases.
        /// @param secondBaseCenter Center of the second base.
        /// @param secondBaseRadius Radius of the second base.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawCutCone(const std::string& id, glm::vec3 firstBaseCenter, float firstBaseRadius,
                         glm::vec3 secondBaseCenter, float secondBaseRadius, float lifespan = 0.0F,
                         Space space = Space::World);

        /// @brief Draws a wireframe box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param corner One of the corners of the box to be drawn.
        /// @param oppositeCorner The opposite corner of the box to be drawn.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawWireBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan = 0.0F,
                         Space space = Space::World);

        /// @brief Class that describes a type of gizmo
        class Gizmo
        {
        public:
            virtual ~Gizmo() = default;

            Gizmo(const std::string& id, glm::vec3 color, float lifespan);

            /// @brief Draws the gizmo to screen.
            /// @param renderer Renderer.
            /// @param mvp The view projection matrix to use for drawing the gizmo.
            virtual void draw(GizmosRenderer& renderer, const glm::mat<4, 4, float, glm::packed_highp>& mvp) = 0;

            /// @brief Decreases the time the gizmo has left before it is destroyed.
            /// @param delta Seconds since the last frame.
            bool decreaseLifespan(float delta);

        protected:
            const std::string& mId; ///< Gizmo identifier.
            glm::vec3 mColor;       ///< Color of the gizmo.
            float mLifespan;        ///< Time in seconds the gizmo has left to live.
        };

        std::vector<std::shared_ptr<Gizmo>> worldGizmos;  ///< Queued gizmos to be drawn in world space.
        std::vector<std::shared_ptr<Gizmo>> viewGizmos;   ///< Queued gizmos to be drawn in viewport space.
        std::vector<std::shared_ptr<Gizmo>> screenGizmos; ///< Queued gizmos to be drawn in screen space.

    private:
        /// @brief Adds a gizmo into the corresponding vector.
        /// @param gizmo Gizmo to be added.
        /// @param space Space in which the gizmo will be drawn.
        void push(const std::shared_ptr<Gizmo>& gizmo, const Space& space);

        glm::vec3 mColor; ///< Currently set color.
    };

} // namespace cubos::engine
