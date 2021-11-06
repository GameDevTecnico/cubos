#ifndef CUBOS_DEBUG_RENDER_HPP
#define CUBOS_DEBUG_RENDER_HPP

#include <list>
#include <mutex>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cubos/gl/render_device.hpp>

namespace cubos
{
    class Debug
    {
    public:
        /// Initialize the debug rendering system.
        static void init(gl::RenderDevice& renderDevice);

        /**
         * @brief
         * Draw a filled cube that will stay visible for a specified amount of time.
         * @param position Position of the cube's center in world space.
         * @param size Size of the cube in the x, y and z dimensions in world space.
         * @param time How long should the cube be visible for?
         * @param rotation Rotation of the cube in world space.
         * @param color Color of the drawn cube.
         */
        static void drawCube(glm::vec3 position, glm::vec3 size, float time, glm::quat rotation = glm::quat(),
                             glm::vec3 color = glm::vec3(1));

        /**
         * @brief
         * Draw a wireframe cube that will stay visible for a specified amount of time.
         * @param position Position of the cube's center in world space.
         * @param size Size of the cube in the x, y and z dimensions in world space.
         * @param time How long should the cube be visible for?
         * @param rotation Rotation of the cube in world space.
         * @param color Color of the drawn cube.
         */
        static void drawWireCube(glm::vec3 position, glm::vec3 size, float time, glm::quat rotation = glm::quat(),
                                 glm::vec3 color = glm::vec3(1));

        /**
         * @brief
         * Draw a filled sphere that will stay visible for a specified amount of time.
         * @param center Position of the sphere's center in world space.
         * @param radius Radius of the sphere.
         * @param time How long should the sphere be visible?
         * @param color Color of the drawn sphere.
         */
        static void drawSphere(glm::vec3 center, float radius, float time, glm::vec3 color = glm::vec3(1));

        /**
         * @brief
         * Draw a wireframe sphere that will stay visible for a specified amount of time.
         * @param center Position of the sphere's center in world space.
         * @param radius Radius of the sphere.
         * @param time How long should the sphere be visible?
         * @param color Color of the drawn sphere.
         */
        static void drawWireSphere(glm::vec3 center, float radius, float time, glm::vec3 color = glm::vec3(1));

        /**
         * @brief
         * Render all objects in the debug renderer's buffer and update said buffer, removing all objects that have
         * exhausted their requested time.
         * @param vp View-Projection matrix that the objects should be drawn with.
         * @param deltaT Time that has passed since the last call to flush.
         */
        static void flush(glm::mat4 vp, double deltaT);

    private:
        struct DebugDrawObject
        {
            gl::VertexArray va = nullptr;
            gl::IndexBuffer ib = nullptr;
            int numIndices;
        };

        struct DebugDrawRequest
        {
            DebugDrawObject& obj;
            gl::RasterState rasterState;
            glm::mat4 modelMatrix;
            double timeLeft;
            glm::vec3 color;
        };

        static void initCube();
        static void initSphere();

        static gl::RenderDevice* renderDevice;
        static gl::ConstantBuffer mvpBuffer;
        static gl::ShaderBindingPoint mvpBindingPoint, colorBindingPoint;
        static gl::ShaderPipeline pipeline;

        static gl::RasterState fillRasterState, wireframeRasterState;
        static DebugDrawObject objCube, objSphere;

        static std::list<DebugDrawRequest> requests;

        static std::mutex debugDrawMutex;
    };
} // namespace cubos

#endif // CUBOS_DEBUG_RENDER_HPP
