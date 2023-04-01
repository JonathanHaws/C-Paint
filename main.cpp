#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    if (!glfwInit()) { std::cout << "Failed to initialize GLFW" << std::endl; return -1;}
    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL GRAPHICS!", NULL, NULL);
    if (!window) { std::cout << "Failed to create window" << std::endl; glfwTerminate(); return -1;}
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}