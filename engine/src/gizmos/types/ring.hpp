#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <cubos/engine/gizmos/gizmos.hpp>

#include "../renderer.hpp"

namespace cubos::engine
{
    /// @brief A gizmo that is a ring.
    class RingGizmo : public Gizmos::Gizmo
    {
        glm::vec3 mPointA;
        glm::vec3 mPointB;
        float mRadiusOuter;
        float mRadiusInner;

    public:
        /// @brief Constructs.
        /// @param id Identifier of the gizmo.
        /// @param firstBasePosition Center of one of the bases.
        /// @param secondBasePosition Center of the second base.
        /// @param outerRadius Radius of one of the ring.
        /// @param innerRadius Radius of the of the hole.
        /// @param color Color for the gizmo to be drawn in.
        /// @param lifespan Time the gizmo will remain on screen, in seconds.
        RingGizmo(uint32_t id, glm::vec3 firstBasePosition, glm::vec3 secondBasePosition, float outerRadius,
                  float innerRadius, const glm::vec3& color, float lifespan)
            : Gizmos::Gizmo(id, color, lifespan)
            , mPointA(firstBasePosition)
            , mPointB(secondBasePosition)
            , mRadiusOuter(outerRadius)
            , mRadiusInner(innerRadius)
        {
        }

        /// @brief Draws the gizmo to screen.
        /// @param renderer GizmosRenderer in use.
        /// @param phase Current draw phase.
        /// @param mvp Matrix containing projection and viewpoint transformations.
        void draw(GizmosRenderer& renderer, DrawPhase phase,
                  const glm::mat<4, 4, float, glm::packed_highp>& mvp) override
        {
            auto* verts = static_cast<glm::vec3*>(renderer.ringPrimitive.vb->map());

            glm::vec3 n = glm::normalize(mPointB - mPointA);
            glm::vec3 p;
            if (n[0] != n[1])
            {
                p = {n[1], -n[0], n[2]};
            }
            else if (n[0] != n[2])
            {
                p = {-n[2], n[1], n[0]};
            }
            else
            {
                p = {n[0], -n[2], n[1]};
            }

            glm::vec3 pA = p * mRadiusOuter;
            glm::vec3 pB = p * mRadiusInner;

            for (std::size_t i = 0; i < GizmosRenderer::RingSections; i++)
            {
                glm::vec3 vertOuter =
                    glm::rotate(pA, (float)i * glm::radians(360.0F / (float)GizmosRenderer::RingSections), n) + mPointA;
                glm::vec3 vertInner =
                    glm::rotate(pB, (float)i * glm::radians(360.0F / (float)GizmosRenderer::RingSections), n) + mPointA;
                verts[2 * i] = {vertOuter[0], vertOuter[1], vertOuter[2]};
                verts[(2 * i) + 1] = {vertInner[0], vertInner[1], vertInner[2]};
            }

            for (std::size_t i = 0; i < GizmosRenderer::RingSections; i++)
            {
                glm::vec3 vertOuter =
                    glm::rotate(pA, (float)i * glm::radians(360.0F / (float)GizmosRenderer::RingSections), n) + mPointB;
                glm::vec3 vertInner =
                    glm::rotate(pB, (float)i * glm::radians(360.0F / (float)GizmosRenderer::RingSections), n) + mPointB;
                verts[(std::size_t)GizmosRenderer::RingSections * 2 + 2 * i] = {vertOuter[0], vertOuter[1],
                                                                                vertOuter[2]};
                verts[(std::size_t)GizmosRenderer::RingSections * 2 + (2 * i) + 1] = {vertInner[0], vertInner[1],
                                                                                      vertInner[2]};
            }

            renderer.ringPrimitive.vb->unmap();

            renderer.renderDevice->setVertexArray(renderer.ringPrimitive.va);
            renderer.renderDevice->setIndexBuffer(renderer.ringPrimitive.ib);

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
            renderer.renderDevice->drawTrianglesIndexed(0,
                                                        24 * (std::size_t)cubos::engine::GizmosRenderer::RingSections);
        }
    };
} // namespace cubos::engine
