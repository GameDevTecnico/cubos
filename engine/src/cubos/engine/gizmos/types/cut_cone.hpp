#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <cubos/engine/gizmos/gizmos.hpp>

#include "../renderer.hpp"

namespace cubos::engine
{
    /// @brief A gizmo that is a cylinder with bases that can be different.
    class CutConeGizmo : public Gizmos::Gizmo
    {
        glm::vec3 mPointA;
        float mRadiusA;
        glm::vec3 mPointB;
        float mRadiusB;

    public:
        /// @brief Constructs.
        /// @param id Identifier of the gizmo.
        /// @param firstBaseCenter Center of one of the bases.
        /// @param firstBaseRadius Radius of one of the bases.
        /// @param secondBaseCenter Center of the second base.
        /// @param secondBaseRadius Radius of the second base.
        /// @param color Color for the gizmo to be drawn in.
        /// @param lifespan Time the gizmo will remain on screen, in seconds.
        CutConeGizmo(uint32_t id, glm::vec3 firstBaseCenter, float firstBaseRadius, glm::vec3 secondBaseCenter,
                     float secondBaseRadius, const glm::vec3& color, float lifespan)
            : Gizmos::Gizmo(id, color, lifespan)
            , mPointA(firstBaseCenter)
            , mRadiusA(firstBaseRadius)
            , mPointB(secondBaseCenter)
            , mRadiusB(secondBaseRadius)
        {
        }

        /// @brief Draws the gizmo to screen.
        /// @param renderer GizmosRenderer in use.
        /// @param phase Current draw phase.
        /// @param mvp Matrix containing projection and viewpoint transformations.
        void draw(GizmosRenderer& renderer, DrawPhase phase,
                  const glm::mat<4, 4, float, glm::packed_highp>& mvp) override
        {
            auto* verts = static_cast<glm::vec3*>(renderer.cutConePrimitive.vb->map());

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

            glm::vec3 pA = p * mRadiusA;

            for (int i = 0; i < GizmosRenderer::CutConeVertsPerBase; i++)
            {
                glm::vec3 vert =
                    glm::rotate(pA, (float)i * glm::radians(360.0F / (float)GizmosRenderer::CutConeVertsPerBase), n) +
                    mPointA;
                verts[i] = {vert[0], vert[1], vert[2]};
            }

            glm::vec3 pB = p * mRadiusB;

            for (int i = GizmosRenderer::CutConeVertsPerBase; i < 2 * GizmosRenderer::CutConeVertsPerBase; i++)
            {
                glm::vec3 vert =
                    glm::rotate(pB, (float)i * glm::radians(360.0F / (float)GizmosRenderer::CutConeVertsPerBase), n) +
                    mPointB;
                verts[i] = {vert[0], vert[1], vert[2]};
            }

            renderer.cutConePrimitive.vb->unmap();

            renderer.renderDevice->setVertexArray(renderer.cutConePrimitive.va);
            renderer.renderDevice->setIndexBuffer(renderer.cutConePrimitive.ib);

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
                                                        12 * cubos::engine::GizmosRenderer::CutConeVertsPerBase - 12);
        }
    };
} // namespace cubos::engine
