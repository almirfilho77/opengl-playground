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

    float positions[] = {
        -0.5f,  -0.5f, //0
         0.5f,  -0.5f, //1
         0.5f,   0.5f, //2
        -0.5f,   0.5f, //3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    // Create a Vertex Array Object
    VertexArray* va = new VertexArray();
    VertexBuffer *vb = new VertexBuffer(positions, 4 * 2 * sizeof(float));
    VertexBufferLayout* vbl = new VertexBufferLayout();
    vbl->Push<float>(2);
    va->AddBuffer(*vb, *vbl);

    // Index buffer object
    IndexBuffer* ib = new IndexBuffer(indices, 6);

    Shader* shader = new Shader("res/shaders/Basic.shader");
    shader->SetUniform4f("u_Color", 0.3f, 0.5f, 0.8f, 1.0f);

    /* To show that we can reuse the state of the created VAO
     * even if we unbind everything now
     */
    va->Unbind();
    vb->Unbind();
    ib->Unbind();
    shader->Unbind();

    float r_channel = 0.0f;
    float g_channel = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCallVoid(glClear(GL_COLOR_BUFFER_BIT));

        // Prepare for draw call
        shader->Bind();
        va->Bind();     // Binding the VAO back, binds back also the vertex buffer and the element buffer that were bound to it before

        // Draw shape
        shader->SetUniform4f("u_Color", r_channel, g_channel, 0.8f, 1.0f);
        GLCallVoid(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); // nullptr here because the buffer is already bound to ibo

        // Animate the color of the shape
        if (r_channel > 1.0f)
        {
            increment = -0.005f;
        }
        else if (r_channel < 0.0f)
        {
            increment = 0.005f;
        }
        r_channel += increment;
        g_channel += increment;

        /* Swap front and back buffers */
        GLCallVoid(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCallVoid(glfwPollEvents());
    }

    delete shader;
    delete ib;
    delete vbl;
    delete vb;
    delete va;

    glfwTerminate();
    return 0;
}