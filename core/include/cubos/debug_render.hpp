#ifndef CUBOS_DEBUG_RENDER_HPP
#define CUBOS_DEBUG_RENDER_HPP

#include <list>
#include <mutex>

#include <glm/glm.hpp>

#include <cubos/gl/render_device.hpp>

namespace cubos
{
    class Debug
    {
    public:
        static void init(gl::RenderDevice& renderDevice);
        static void drawCube(glm::vec3 center, glm::vec3 size, float time, glm::vec3 color = glm::vec3(1));
        static void drawWireCube(glm::vec3 center, glm::vec3 size, float time, glm::vec3 color = glm::vec3(1));
        static void drawSphere(glm::vec3 center, float radius, float time, glm::vec3 color = glm::vec3(1));
        static void drawWireSphere(glm::vec3 center, float radius, float time, glm::vec3 color = glm::vec3(1));
        static void flush(glm::mat4 vp, double deltaT);

    private:
        static gl::RenderDevice* renderDevice;
        static gl::ConstantBuffer mvpBuffer;
        static gl::ShaderBindingPoint mvpBindingPoint, colorBindingPoint;
        static gl::ShaderPipeline pipeline;

        static gl::RasterState fillRasterState, wireframeRasterState;

        struct DebugDrawObject
        {
            gl::VertexArray va = nullptr;
            gl::IndexBuffer ib = nullptr;
            int numIndices;
        };

        static DebugDrawObject objCube, objSphere;

        struct DebugDrawRequest
        {
            DebugDrawObject& obj;
            gl::RasterState rasterState;
            glm::mat4 modelMatrix;
            double timeLeft;
            glm::vec3 color;
        };

        static std::list<DebugDrawRequest> requests;

        static std::mutex debug_draw_mutex;

        static void initCube();
        static void initSphere();
    };
} // namespace cubos

#endif // CUBOS_DEBUG_RENDER_HPP
