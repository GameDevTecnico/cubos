#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include <cubos/engine/gizmos/gizmos.hpp>

#include "../renderer.hpp"

namespace cubos::engine
{
    /// @brief A gizmo that is a line connecting two points.
    class LineGizmo : public cubos::engine::Gizmos::Gizmo
    {
        glm::vec3 mPointA;
        glm::vec3 mPointB;

    public:
        /// @brief Constructs.
        /// @param id Identifier of the gizmo.
        /// @param from Point at one of the ends of the line.
        /// @param to Point at the other end of the line.
        /// @param color Color for the gizmo to be drawn in.
        /// @param lifespan Time the gizmo will remain on screen, in seconds.
        /// @param drawCameras List of camera entities the gizmo will be drawn for. If empty, draws to all cameras.
        LineGizmo(uint32_t id, glm::vec3 from, glm::vec3 to, const glm::vec3& color, float lifespan,
                  std::vector<core::ecs::Entity> drawCameras)
            : cubos::engine::Gizmos::Gizmo(id, color, lifespan, std::move(drawCameras))
            , mPointA(from)
            , mPointB(to)
        {
        }

        /// @brief Draws the gizmo to the screen.
        /// @param renderer GizmosRenderer in use.
        /// @param phase Current draw phase.
        /// @param mvp Matrix containing projection and viewpoint transformations.
        void draw(cubos::engine::GizmosRenderer& renderer, DrawPhase phase,
                  const glm::mat<4, 4, float, glm::packed_highp>& mvp) override
        {
            glm::vec3 verts[2] = {mPointA, mPointB};
            renderer.linePrimitive.vb->fill(verts, sizeof(verts));

            renderer.renderDevice->setVertexArray(renderer.linePrimitive.va);

            auto mvpBuffer =
                renderer.renderDevice->createConstantBuffer(sizeof(glm::mat4), &mvp, cubos::core::gl::Usage::Dynamic);

            if (phase == DrawPhase::Color)
            {
                renderer.drawPipeline->getBindingPoint("MVP")->bind(mvpBuffer);

                renderer.drawPipeline->getBindingPoint("objColor")->setConstant(mColor);
            }
            else
            {
                renderer.idPipeline->getBindingPoint("MVP")->bind(mvpBuffer);
            }

            renderer.renderDevice->setRasterState(
                renderer.renderDevice->createRasterState(cubos::core::gl::RasterStateDesc{}));
            renderer.renderDevice->drawLines(0, 2);
        }
    };
} // namespace cubos::engine
