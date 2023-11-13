#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(const unsigned int* indices, unsigned int count)
    :   m_count(count)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));

    // Generate an internal buffer and assign an index to it
    GLCallVoid(glGenBuffers(1, &m_rendererId));

    // Select the kind of buffer. In this case, an array of memory
    GLCallVoid(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId));

    // Create the actual buffer of data, specifying at least its size
    GLCallVoid(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GLCallVoid(glDeleteBuffers(1, &m_rendererId));
}

void IndexBuffer::Bind() const
{
    GLCallVoid(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererId));
}

void IndexBuffer::Unbind() const
{
    GLCallVoid(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
