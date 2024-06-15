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
            glm::vec3 n = glm::normalize(mPointB - mPointA);
            glm::vec3 p;
            if (n[0] != 0 && n[1] != 0)
            {
                p = {-n[1], n[0], 0};
            }
            else if (n[0] == 0 && n[1] != 0)
            {
                if (n[2] != 0)
                {
                    p = {1, 0, -n[1] / n[2]};
                }
                else
                {
                    p = {1, 0, 0};
                }
            }
            else
            {
                if (n[2] != 0)
                {
                    p = {0, 1, -n[1] / n[2]};
                }
                else
                {
                    p = {0, 1, 0};
                }
            }

            p = glm::normalize(p);

            glm::vec3 pA = p * mRadiusOuter;
            glm::vec3 pB = p * mRadiusInner;

            glm::vec3 verts[4 * GizmosRenderer::RingSections];
            for (std::size_t i = 0; i < GizmosRenderer::RingSections; i++)
            {
                verts[2 * i] =
                    glm::rotate(pA, (float)i * glm::radians(360.0F / (float)GizmosRenderer::RingSections), n) + mPointA;
                verts[(2 * i) + 1] =
                    glm::rotate(pB, (float)i * glm::radians(360.0F / (float)GizmosRenderer::RingSections), n) + mPointA;
            }

            for (std::size_t i = 0; i < GizmosRenderer::RingSections; i++)
            {
                verts[(std::size_t)GizmosRenderer::RingSections * 2 + 2 * i] =
                    glm::rotate(pA, (float)i * glm::radians(360.0F / (float)GizmosRenderer::RingSections), n) + mPointB;
                verts[(std::size_t)GizmosRenderer::RingSections * 2 + (2 * i) + 1] =
                    glm::rotate(pB, (float)i * glm::radians(360.0F / (float)GizmosRenderer::RingSections), n) + mPointB;
            }
            renderer.ringPrimitive.vb->fill(verts, sizeof(verts));

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
