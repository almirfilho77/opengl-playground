#include <iostream>
#include <string>
#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"


int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);                    // Need to specify in order for the other hints to work
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);                    // Need to specify in order for the other hints to work
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);  // It implicitly creates and binds a VAO
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);    // Need to explicitly create and bind VAO

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

    glfwSwapInterval(1);

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cout << "Error: " << glewGetErrorString(error) << "\n";
        return -1;
    }
    std::cout << "GLEW VERSION = " << glewGetString(GLEW_VERSION) << "\n";

    // Enable alpha channel rendering
    GLCallVoid(glEnable(GL_BLEND));
    GLCallVoid(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    {
        float positions[] = {
            -0.5f,  -0.5f, 0.0f, 0.0f, //0
             0.5f,  -0.5f, 1.0f, 0.0f, //1
             0.5f,   0.5f, 1.0f, 1.0f, //2
            -0.5f,   0.5f, 0.0f, 1.0f, //3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0,
        };

        // Create a Vertex Array Object
        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));
        
        VertexBufferLayout vbl;
        vbl.Push<float>(2);
        vbl.Push<float>(2);
        va.AddBuffer(vb, vbl);

        // Index buffer object
        IndexBuffer ib(indices, 6);

        Shader shader("res/shaders/Basic.shader");
        //shader.SetUniform4f("u_Color", 0.3f, 0.5f, 0.8f, 1.0f);

        Texture texture("res/textures/ronaldinho.png");
        unsigned int slot = 0;
        texture.Bind(slot);
        shader.SetUniform1i("u_Texture", slot);

        /* To show that we can reuse the state of the created VAO
         * even if we unbind everything now
         */
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            // Prepare for draw call
            shader.Bind();
            // Draw shape
            renderer.Draw(va, ib, shader);

            /* Swap front and back buffers */
            GLCallVoid(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCallVoid(glfwPollEvents());
        }
    }

    glfwTerminate();
    return 0;
}