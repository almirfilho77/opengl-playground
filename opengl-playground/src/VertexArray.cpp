#include "VertexArray.h"

#include "VertexBufferLayout.h"

VertexArray::VertexArray()
{
	GLCallVoid(glCreateVertexArrays(1, &m_rendererId));
	GLCallVoid(glBindVertexArray(m_rendererId));
}

VertexArray::~VertexArray()
{
	GLCallVoid(glDeleteVertexArrays(1, &m_rendererId));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();
	unsigned int offset = 0;
	const auto& elements = layout.GetElements();
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		GLCallVoid(glEnableVertexAttribArray(i));
		GLCallVoid(glVertexAttribPointer(i, element.count, element.type, element.normalized, 
			layout.GetStride(), (const void*)offset));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	GLCallVoid(glBindVertexArray(m_rendererId));
}

void VertexArray::Unbind() const
{
	GLCallVoid(glBindVertexArray(0));
}
