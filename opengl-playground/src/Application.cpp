#include <iostream>
#include <string>
#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
    int width = 960;
    int height = 540;
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
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
    
    // Create orthographic projection matrix to compensate the aspect ratio
    // (4x3) of the window when drawing a squared shape.
    // When the shape (or texture) is already rectangular, applying the first matrix 
    // makes it squared.
    // Actually, it transforms every pixel in the texture from the pixel space to the window space.
    // 
    glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(400.0f, 0, 0));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 200.0f, 0.0f));
    glm::mat4 mvp = proj * view * model;

    {
        float positions[] = {
             200.0f,  50.0f,  1.0f, 0.0f, //1
             200.0f,  150.0f, 1.0f, 1.0f, //2
             50.0f,   150.0f, 0.0f, 1.0f, //3
             50.0f,   50.0f,  0.0f, 0.0f, //0
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

        Texture texture("res/textures/ronaldinho.png");
        unsigned int slot = 0;
        texture.Bind(slot);
        shader.SetUniform1i("u_Texture", slot);
        shader.SetUniformMat4f("u_MVP", mvp);

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
            shader.SetUniformMat4f("u_MVP", mvp);

            /* Render here */
            renderer.Clear();

            // Prepare for draw call
            shader.Bind();

            // Draw shape with texture
            renderer.Draw(va, ib, shader);

            // Draw shape (Blending example: blend a full opaque red square with a slight 
            // translucid blue one). 
            // NOTE: change the shader code so color = u_Color
            /*shader.SetUniform4f("u_Color", 1.0f, 0.0f, 0.0f, 1.0f);
            renderer.Draw(va, ib, shader);
            shader.SetUniform4f("u_Color", 0.0f, 0.0f, 1.0f, 0.4f);
            renderer.Draw(va, ib, shader);*/

            /* Swap front and back buffers */
            GLCallVoid(glfwSwapBuffers(window));

            /* Poll for and process events */
            GLCallVoid(glfwPollEvents());
        }
    }

    glfwTerminate();
    return 0;
}