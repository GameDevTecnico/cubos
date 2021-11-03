#ifndef CUBOS_DEBUG_RENDER_HPP
#define CUBOS_DEBUG_RENDER_HPP

#include <cubos/io/window.hpp>
#include <glm/glm.hpp>

namespace cubos
{
    class debug
    {
    public:
        static void init(cubos::io::Window *window);
        static void drawCube(glm::mat4 vp, glm::vec3 center, glm::vec3 size, float time);
        static void drawWireCube(glm::mat4 vp, glm::vec3 center, glm::vec3 size, float time);
        static void drawSphere(glm::mat4 vp, glm::vec3 center, float radius, float time);
        static void drawWireSphere(glm::mat4 vp, glm::vec3 center, float radius, float time);
        static void flush(float deltaT);
    };
} // namespace cubos

#endif // CUBOS_DEBUG_RENDER_HPP