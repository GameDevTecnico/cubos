#pragma once
#include <vector>

#include <glm/glm.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/cubos.hpp>

using cubos::core::gl::RenderDevice;
using cubos::core::gl::ShaderPipeline;
using cubos::engine::DeltaTime;

namespace cubos::engine
{
    class Gizmos final
    {
    public:
        void init(RenderDevice& renderDevice);

        void color(const glm::vec3& color);
        void drawLine(std::string id, glm::vec3 from, glm::vec3 to, float lifespan = 0);

        void drawQueuedGizmos(DeltaTime deltaTime);

    private:
        ShaderPipeline mPipeline;
        RenderDevice* mRenderDevice;
        glm::vec3 mColor;
        class GizmoBase;
        class LineGizmo;
        std::vector<std::shared_ptr<GizmoBase>> mGizmosVector;
    };

} // namespace cubos::engine
