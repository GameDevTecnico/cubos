#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

#include <cubos/log.hpp>

int main(void)
{
    cubos::initializeLogger();
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "GLFW CMake starter", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glClearColor(1, 0, 0, 0.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.894f, 0.592f, 0.141f, 0.0f);
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
