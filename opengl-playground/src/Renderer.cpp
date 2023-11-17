#include "Renderer.h"

#include <iostream>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "OpenGL Error (" << error << ") > " << function << " [" << file << ":" << line << "]" << "\n";
        return false;
    }
    return true;
}

void Renderer::Clear()
{
    GLCallVoid(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader)
{
    va.Bind();      // Binding the VAO back, binds back also the vertex buffer and the element buffer that were bound to it before
    ib.Bind();      // It is a good idea to have an independent buffer array bound at draw call, apparently
    GLCallVoid(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr)); // nullptr here because the buffer is already bound to ibo
}
