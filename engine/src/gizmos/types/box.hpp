#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include <cubos/engine/gizmos/gizmos.hpp>

#include "../renderer.hpp"

namespace cubos::engine
{
    /// @brief A gizmo that is a filled box.
    class BoxGizmo : public cubos::engine::Gizmos::Gizmo
    {
        glm::vec3 mPointA;
        glm::vec3 mPointB;

    public:
        /// @brief Constructs.
        /// @param id Identifier of the gizmo.
        /// @param corner Point at one of the corners of the box.
        /// @param oppositeCorner Point at the opposite corner of the box.
        /// @param color Color for the gizmo to be drawn in.
        /// @param lifespan Time the gizmo will remain on screen, in seconds.
        BoxGizmo(uint32_t id, glm::vec3 corner, glm::vec3 oppositeCorner, const glm::vec3& color, float lifespan)
            : cubos::engine::Gizmos::Gizmo(id, color, lifespan)
            , mPointA(corner)
            , mPointB(oppositeCorner)
        {
        }

        /// @brief Draws the gizmo to the screen.
        /// @param renderer GizmosRenderer in use.
        /// @param phase Current draw phase.
        /// @param mvp Matrix containing projection and viewpoint transformations.
        void draw(cubos::engine::GizmosRenderer& renderer, DrawPhase phase,
                  const glm::mat<4, 4, float, glm::packed_highp>& mvp) override
        {
            auto* verts = static_cast<glm::vec3*>(renderer.boxPrimitive.vb->map());
            verts[0] = {mPointA[0], mPointA[1], mPointA[2]};
            verts[1] = {mPointB[0], mPointA[1], mPointA[2]};
            verts[2] = {mPointB[0], mPointA[1], mPointB[2]};
            verts[3] = {mPointA[0], mPointA[1], mPointB[2]};
            verts[4] = {mPointA[0], mPointB[1], mPointB[2]};
            verts[5] = {mPointA[0], mPointB[1], mPointA[2]};
            verts[6] = {mPointB[0], mPointB[1], mPointA[2]};
            verts[7] = {mPointB[0], mPointB[1], mPointB[2]};

            renderer.boxPrimitive.vb->unmap();

            renderer.renderDevice->setVertexArray(renderer.boxPrimitive.va);
            renderer.renderDevice->setIndexBuffer(renderer.boxPrimitive.ib);

            auto mvpBuffer =
                renderer.renderDevice->createConstantBuffer(sizeof(glm::mat4), &mvp, cubos::core::gl::Usage::Static);

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
            renderer.renderDevice->drawTrianglesIndexed(0, 36);
        }
    };
} // namespace cubos::engine
