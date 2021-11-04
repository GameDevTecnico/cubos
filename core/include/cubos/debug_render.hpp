#ifndef CUBOS_DEBUG_RENDER_HPP
#define CUBOS_DEBUG_RENDER_HPP

#include <cubos/io/window.hpp>
#include <glm/glm.hpp>

namespace cubos
{
    class Debug
    {
    public:
        static void init(cubos::io::Window* window);
        static void drawCube(glm::vec3 center, glm::vec3 size, float time, glm::vec3 color = glm::vec3(1));
        static void drawWireCube(glm::vec3 center, glm::vec3 size, float time, glm::vec3 color = glm::vec3(1));
        static void drawSphere(glm::vec3 center, float radius, float time, glm::vec3 color = glm::vec3(1));
        static void drawWireSphere(glm::vec3 center, float radius, float time, glm::vec3 color = glm::vec3(1));
        static void flush(glm::mat4 vp, double deltaT);
    };
} // namespace cubos

#endif // CUBOS_DEBUG_RENDER_HPP
