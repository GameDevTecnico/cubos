/// @file
/// @brief Resource @ref cubos::engine::Gizmos.
/// @ingroup gizmos-plugin

#pragma once

#include <functional>
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

        /// @brief Draws an arrow gizmo.
        /// @param id Identifier of the gizmo.
        /// @param origin Point from which the arrow starts.
        /// @param direction Direction of the arrow.
        /// @param girth Width of the cylinder part of the arrow.
        /// @param width Width of the base of the cone at the tip of the arrow.
        /// @param ratio Point of the arrow at which the cylinder ends and the cone begins.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawArrow(const std::string& id, glm::vec3 origin, glm::vec3 direction, float girth, float width,
                       float ratio = 0.F, float lifespan = 0.0F, Space space = Space::World);

        /// @brief Draws a wireframe box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param corner One of the corners of the box to be drawn.
        /// @param oppositeCorner The opposite corner of the box to be drawn.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawWireBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner, float lifespan = 0.0F,
                         Space space = Space::World);

        /// @brief Gets whether the left mouse button was pressed over a gizmo.
        /// @param id Identifier of the gizmo.
        /// @return Whether the gizmo was pressed.
        bool pressed(const std::string& id) const;

        /// @brief Gets whether the mouse button is over a gizmo.
        /// @param id Identifier of the gizmo.
        /// @return Whether the mouse button is over a gizmo.
        bool hover(const std::string& id) const;

        void registerInputGizmo(unsigned int id, bool pressed);

        /// @brief Class that describes a type of gizmo
        class Gizmo
        {
        public:
            enum class DrawPhase
            {
                Color,
                Id
            };

            virtual ~Gizmo() = default;

            Gizmo(unsigned int id, glm::vec3 color, float lifespan);

            /// @brief Draws the gizmo to screen.
            /// @param renderer Renderer.
            /// @param mvp The view projection matrix to use for drawing the gizmo.
            virtual void draw(GizmosRenderer& renderer, DrawPhase phase,
                              const glm::mat<4, 4, float, glm::packed_highp>& mvp) = 0;

            /// @brief Decreases the time the gizmo has left before it is destroyed.
            /// @param delta Seconds since the last frame.
            bool decreaseLifespan(float delta);

            unsigned int id; ///< Gizmo identifier.

        protected:
            glm::vec3 mColor; ///< Color of the gizmo.
            float mLifespan;  ///< Time in seconds the gizmo has left to live.
        };

        std::vector<std::shared_ptr<Gizmo>> worldGizmos;  ///< Queued gizmos to be drawn in world space.
        std::vector<std::shared_ptr<Gizmo>> viewGizmos;   ///< Queued gizmos to be drawn in viewport space.
        std::vector<std::shared_ptr<Gizmo>> screenGizmos; ///< Queued gizmos to be drawn in screen space.

    private:
        /// @brief Adds a gizmo into the corresponding vector.
        /// @param gizmo Gizmo to be added.
        /// @param space Space in which the gizmo will be drawn.
        void push(const std::shared_ptr<Gizmo>& gizmo, const Space& space);

        std::hash<std::string> mHasher; ///< Hash function to convert string ids into integers.

        glm::vec3 mColor; ///< Currently set color.

        unsigned int mIdInteraction; ///< Interaction target gizmo.
        bool mPressed;               ///< Whether the mouse has been pressed.
    };

} // namespace cubos::engine
