#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(void)
{
    GLFWwindow* window;

    // Initialize the library 
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                      // Need to specify in order for the other hints to work
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                      // Need to specify in order for the other hints to work
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);  // It implicitly creates and binds a VAO
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);      // Need to explicitly create and bind VAO

    // Create a windowed mode window and its OpenGL context 
    int width = 800;
    int height = 600;
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cout << "Error: could not create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    // Make the window's context current 
    glfwMakeContextCurrent(window);
    std::cout << "GL VERSION = " << glGetString(GL_VERSION) << "\n";

    // Initialize GLEW 
    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cout << "Error: " << glewGetErrorString(error) << "\n";
        return -1;
    }
    std::cout << "GLEW VERSION = " << glewGetString(GLEW_VERSION) << "\n";

    // Specify the render area of the window  
    glViewport(0, 0, width, height);

    // Base color of the window
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Swap front and back buffers 
    glfwSwapBuffers(window);
        
    // Loop until the user closes the window 
    while (!glfwWindowShouldClose(window))
    {
        // Poll for and process events 
        glfwPollEvents();
    }

    // Destroy the window
    glfwDestroyWindow(window);
    
    // Terminate GLFW
    glfwTerminate();
    return 0;
}