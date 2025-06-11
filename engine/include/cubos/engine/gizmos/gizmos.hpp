/// @file
/// @brief Resource @ref cubos::engine::Gizmos.
/// @ingroup gizmos-plugin

#pragma once

#include <functional>
#include <vector>

#include <glm/vec3.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/prelude.hpp>

namespace cubos::engine
{
    class GizmosRenderer;

    /// @brief Resource which queues commands for drawing gizmos, basic primitives useful for debugging and tools.
    ///
    /// @ingroup gizmos-plugin
    class CUBOS_ENGINE_API Gizmos final
    {
    public:
        CUBOS_REFLECT;

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
        /// @param drawCameras List of camera entities the gizmo will be drawn for. If empty, draws to all cameras.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawLine(const std::string& id, glm::vec3 from, glm::vec3 to,
                      const std::vector<core::ecs::Entity>& drawCameras = {}, float lifespan = 0.0F,
                      Space space = Space::World);

        /// @brief Draws a filled box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param corner One of the corners of the box to be drawn.
        /// @param oppositeCorner The opposite corner of the box to be drawn.
        /// @param drawCameras List of camera entities the gizmo will be drawn for. If empty, draws to all cameras.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner,
                     const std::vector<core::ecs::Entity>& drawCameras = {}, float lifespan = 0.0F,
                     Space space = Space::World);

        /// @brief Draws a filled box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param transform Transformation matrix to apply to a unit-sized box centered at the origin.
        /// @param drawCameras List of camera entities the gizmo will be drawn for. If empty, draws to all cameras.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawBox(const std::string& id, const glm::mat4& transform,
                     const std::vector<core::ecs::Entity>& drawCameras = {}, float lifespan = 0.0F,
                     Space space = Space::World);

        /// @brief Draws a cut cone gizmo.
        /// @param id Identifier of the gizmo.
        /// @param firstBaseCenter Center of one of the bases.
        /// @param firstBaseRadius Radius of one of the bases.
        /// @param secondBaseCenter Center of the second base.
        /// @param secondBaseRadius Radius of the second base.
        /// @param drawCameras List of camera entities the gizmo will be drawn for. If empty, draws to all cameras.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawCutCone(const std::string& id, glm::vec3 firstBaseCenter, float firstBaseRadius,
                         glm::vec3 secondBaseCenter, float secondBaseRadius,
                         const std::vector<core::ecs::Entity>& drawCameras = {}, float lifespan = 0.0F,
                         Space space = Space::World);

        /// @brief Draws a ring gizmo.
        /// @param id Identifier of the gizmo.
        /// @param firstBasePosition Center of one of the bases.
        /// @param secondBasePosition Center of the second base.
        /// @param outerRadius Radius of one of the ring.
        /// @param innerRadius Radius of the of the hole.
        /// @param drawCameras List of camera entities the gizmo will be drawn for. If empty, draws to all cameras.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawRing(const std::string& id, glm::vec3 firstBasePosition, glm::vec3 secondBasePosition,
                      float outerRadius, float innerRadius, const std::vector<core::ecs::Entity>& drawCameras = {},
                      float lifespan = 0.0F, Space space = Space::World);

        /// @brief Draws an arrow gizmo.
        /// @param id Identifier of the gizmo.
        /// @param origin Point from which the arrow starts.
        /// @param direction Direction of the arrow.
        /// @param girth Radius of the cylinder part of the arrow.
        /// @param width Radius of the base of the cone at the tip of the arrow.
        /// @param ratio Point of the arrow at which the cylinder ends and the cone begins.
        /// @param drawCameras List of camera entities the gizmo will be drawn for. If empty, draws to all cameras.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawArrow(const std::string& id, glm::vec3 origin, glm::vec3 direction, float girth, float width,
                       float ratio = 0.F, const std::vector<core::ecs::Entity>& drawCameras = {}, float lifespan = 0.0F,
                       Space space = Space::World);

        /// @brief Draws a wireframe box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param corner One of the corners of the box to be drawn.
        /// @param oppositeCorner The opposite corner of the box to be drawn.
        /// @param drawCameras List of camera entities the gizmo will be drawn for. If empty, draws to all cameras.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawWireBox(const std::string& id, glm::vec3 corner, glm::vec3 oppositeCorner,
                         const std::vector<core::ecs::Entity>& drawCameras = {}, float lifespan = 0.0F,
                         Space space = Space::World);

        /// @brief Draws a wireframe box gizmo.
        /// @param id Identifier of the gizmo.
        /// @param transform Transformation matrix to apply to a unit-sized box centered at the origin.
        /// @param drawCameras List of camera entities the gizmo will be drawn for. If empty, draws to all cameras.
        /// @param lifespan How long the line will be on screen for, in seconds. Defaults to 0, which means a single
        /// frame.
        /// @param space Space to draw the gizmo in.
        void drawWireBox(const std::string& id, const glm::mat4& transform,
                         const std::vector<core::ecs::Entity>& drawCameras = {}, float lifespan = 0.0F,
                         Space space = Space::World);

        /// @brief Checks whether the left mouse button was pressed over a gizmo.
        /// @param id Identifier of the gizmo.
        /// @return Whether the gizmo was pressed.
        bool pressed(const std::string& id) const;

        /// @brief Checks whether the left mouse button was pressed over a gizmo.
        /// @param id Identifier of the gizmo.
        /// @return Whether the gizmo was pressed.
        bool locked(const std::string& id) const;

        /// @brief Checks whether the mouse cursor is over a gizmo.
        /// @param id Identifier of the gizmo.
        /// @return Whether the mouse button is over a gizmo.
        bool hovered(const std::string& id) const;

        /// @brief Notifies that a given gizmo is being hovered. Called automatically by an internal system.
        /// @param hovered Gizmo being hovered.
        /// @param pressed Whether the mouse left button is pressed.
        void handleInput(uint32_t hovered, bool pressed);

        /// @brief Notifies that a given gizmo is locked.
        /// @param locked Gizmo being locked.
        void setLocked(uint32_t locked);

        /// @brief Notifies that the previously locked gizmo is no longer locked.
        void releaseLocked();

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

            Gizmo(uint32_t id, glm::vec3 color, float lifespan, std::vector<core::ecs::Entity> drawCameras);

            /// @brief Draws the gizmo to screen.
            /// @param renderer Renderer.
            /// @param mvp The view projection matrix to use for drawing the gizmo.
            virtual void draw(GizmosRenderer& renderer, DrawPhase phase,
                              const glm::mat<4, 4, float, glm::packed_highp>& mvp) = 0;

            /// @brief Decreases the time the gizmo has left before it is destroyed.
            /// @param delta Seconds since the last frame.
            bool decreaseLifespan(float delta);

            const uint32_t id; ///< Gizmo identifier.

            std::vector<core::ecs::Entity>
                drawCameras; ///< List of camera entities the gizmo will be drawn for. If empty, draws to all cameras.

        protected:
            glm::vec3 mColor; ///< Color of the gizmo.
            float mLifespan;  ///< Time in seconds the gizmo has left to live.
        };

        std::vector<std::shared_ptr<Gizmo>> worldGizmos;  ///< Queued gizmos to be drawn in world space.
        std::vector<std::shared_ptr<Gizmo>> viewGizmos;   ///< Queued gizmos to be drawn in viewport space.
        std::vector<std::shared_ptr<Gizmo>> screenGizmos; ///< Queued gizmos to be drawn in screen space.

        bool mLocking; ///< Whether the mouse has just now been pressed.

    private:
        /// @brief Adds a gizmo into the corresponding vector.
        /// @param gizmo Gizmo to be added.
        /// @param space Space in which the gizmo will be drawn.
        void push(const std::shared_ptr<Gizmo>& gizmo, const Space& space);

        static uint32_t mHasher(const std::string& id); ///< Hash function to convert string ids into integers.

        glm::vec3 mColor; ///< Currently set color.

        uint32_t mIdInteraction; ///< Interaction target gizmo.
        bool mPressed;           ///< Whether the mouse has been pressed.

        uint32_t mIdInteractionLocked; ///< Interaction target gizmo.
        bool mLocked;                  ///< Whether the mouse has been pressed.
    };
} // namespace cubos::engine
