#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    // Generate an internal buffer and assign an index to it
    GLCallVoid(glGenBuffers(1, &m_rendererId));

    // Select the kind of buffer. In this case, an array of memory
    GLCallVoid(glBindBuffer(GL_ARRAY_BUFFER, m_rendererId));

    // Create the actual buffer of data, specifying at least its size
    GLCallVoid(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
    GLCallVoid(glDeleteBuffers(1, &m_rendererId));
}

void VertexBuffer::Bind() const
{
    GLCallVoid(glBindBuffer(GL_ARRAY_BUFFER, m_rendererId));
}

void VertexBuffer::Unbind() const
{
    GLCallVoid(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
