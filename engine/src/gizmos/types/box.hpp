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
        glm::mat4 mTransform;

    public:
        /// @brief Constructs.
        /// @param id Identifier of the gizmo.
        /// @param corner Point at one of the corners of the box.
        /// @param oppositeCorner Point at the opposite corner of the box.
        /// @param color Color for the gizmo to be drawn in.
        /// @param lifespan Time the gizmo will remain on screen, in seconds.
        BoxGizmo(uint32_t id, glm::vec3 corner, glm::vec3 oppositeCorner, glm::mat4 transform, const glm::vec3& color,
                 float lifespan)
            : cubos::engine::Gizmos::Gizmo(id, color, lifespan)
            , mPointA(corner)
            , mPointB(oppositeCorner)
            , mTransform(transform)
        {
        }

        /// @brief Draws the gizmo to the screen.
        /// @param renderer GizmosRenderer in use.
        /// @param phase Current draw phase.
        /// @param mvp Matrix containing projection and viewpoint transformations.
        void draw(cubos::engine::GizmosRenderer& renderer, DrawPhase phase,
                  const glm::mat<4, 4, float, glm::packed_highp>& mvp) override
        {
            glm::vec3 verts[8] = {
                mTransform * glm::vec4{mPointA[0], mPointA[1], mPointA[2], 1.0F},
                mTransform * glm::vec4{mPointB[0], mPointA[1], mPointA[2], 1.0F},
                mTransform * glm::vec4{mPointB[0], mPointA[1], mPointB[2], 1.0F},
                mTransform * glm::vec4{mPointA[0], mPointA[1], mPointB[2], 1.0F},
                mTransform * glm::vec4{mPointA[0], mPointB[1], mPointB[2], 1.0F},
                mTransform * glm::vec4{mPointA[0], mPointB[1], mPointA[2], 1.0F},
                mTransform * glm::vec4{mPointB[0], mPointB[1], mPointA[2], 1.0F},
                mTransform * glm::vec4{mPointB[0], mPointB[1], mPointB[2], 1.0F},
            };
            renderer.boxPrimitive.vb->fill(verts, sizeof(verts));

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
    }; // namespace cubos::engine
} // namespace cubos::engine
