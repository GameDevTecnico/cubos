/// @file
/// @brief Class @ref cubos::core::gl::Debug.
/// @ingroup core-gl

#pragma once

#include <list>
#include <mutex>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cubos/core/geom/box.hpp>
#include <cubos/core/gl/render_device.hpp>

namespace cubos::core::gl
{
    /// @brief Singleton with static methods used to draw primitive objects on screen for debugging
    /// purposes.
    /// @ingroup core-gl
    class CUBOS_CORE_API Debug
    {
    public:
        /// @brief Initializes the debug rendering system.
        /// @param renderDevice Render device to use.
        static void init(RenderDevice& renderDevice);

        /// @brief Draws a line that will stay visible for a specified amount of time.
        /// @param start Starting point of the line.
        /// @param end Ending point of the line.
        /// @param relative Whether the ending point is relative to the starting point.
        /// @param color Color of the drawn line.
        /// @param time How long will it be visible for? If 0, for a single frame.
        static void drawLine(glm::vec3 start, glm::vec3 end, bool relative = false, glm::vec3 color = glm::vec3(1),
                             float time = 0.0F);

        /// @brief Draws a filled box that will stay visible for a specified amount of time.
        /// @param box Box to draw.
        /// @param transform Transformation matrix to apply to the box.
        /// @param color Color of the drawn cube.
        /// @param time How long will it be visible for? If 0, for a single frame.
        static void drawBox(geom::Box box, glm::mat4 transform, glm::vec3 color = glm::vec3(1), float time = 0);

        /// @brief Draws a wireframe box that will stay visible for a specified amount of time.
        /// @param box Box to draw.
        /// @param transform Transformation matrix to apply to the box.
        /// @param color Color of the drawn cube.
        /// @param time How long will it be visible for? If 0, for a single frame.
        static void drawWireBox(geom::Box box, glm::mat4 transform, glm::vec3 color = glm::vec3(1), float time = 0);

        /// @brief Draws a filled sphere that will stay visible for a specified amount of time.
        /// @param center Position of the sphere's center in world space.
        /// @param radius Radius of the sphere.
        /// @param time How long will it be visible for? If 0, for a single frame.
        /// @param color Color of the drawn sphere.
        static void drawSphere(glm::vec3 center, float radius, float time, glm::vec3 color = glm::vec3(1));

        /// @brief Draws a wireframe sphere that will stay visible for a specified amount of time.
        /// @param center Position of the sphere's center in world space.
        /// @param radius Radius of the sphere.
        /// @param time How long will it be visible for? If 0, for a single frame.
        /// @param color Color of the drawn sphere.
        static void drawWireSphere(glm::vec3 center, float radius, float time, glm::vec3 color = glm::vec3(1));

        /// @brief Renders all objects in the debug renderer's buffer and removes all objects that
        /// have exhausted their requested time.
        /// @param vp View-Projection matrix that the objects should be drawn with.
        /// @param deltaT Time that has passed since the last call to flush.
        static void flush(glm::mat4 vp, double deltaT);

        /// @brief Cleans up the debug rendering system's resources.
        static void terminate();

    private:
        struct DebugDrawObject
        {
            gl::VertexArray va = nullptr;
            gl::IndexBuffer ib = nullptr;
            unsigned int numIndices;

            void clear();
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
        static gl::ShaderBindingPoint mvpBindingPoint;
        static gl::ShaderBindingPoint colorBindingPoint;
        static gl::ShaderPipeline pipeline;

        static gl::RasterState fillRasterState;
        static gl::RasterState wireframeRasterState;
        static DebugDrawObject objCube;
        static DebugDrawObject objSphere;

        static std::list<DebugDrawRequest> requests;

        static std::mutex debugDrawMutex;
    };
} // namespace cubos::core::gl
