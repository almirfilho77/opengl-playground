#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cout << "Error: could not create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    std::cout << "GL VERSION = " << glGetString(GL_VERSION) << "\n";

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cout << "Error: " << glewGetErrorString(error) << "\n";
        return -1;
    }
    std::cout << "GLEW VERSION = " << glewGetString(GLEW_VERSION) << "\n";

    float positions[6] = {
        -0.5f,  -0.5f,
         0.5f,  -0.5f,
         0.0f,   0.5f,
    };

    // This is the index of the buffer of data that we will create
    unsigned int buffer_index;                      

    // Generate an internal buffer and assign an index to it
    glGenBuffers(1, &buffer_index);         

    // Select the kind of buffer. In this case, an array of memory
    glBindBuffer(GL_ARRAY_BUFFER, buffer_index); 

    // Create the actual buffer of data, specifying at least its size
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    // Set the specification of the attribute "vertex position" in the vertex for the buffer
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void *)0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Draw rectangle
        /*glBegin(GL_QUADS);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);*/

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}