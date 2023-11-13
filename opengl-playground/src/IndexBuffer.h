#pragma once

#include "Renderer.h"

class IndexBuffer
{
private:
	unsigned int m_rendererId;
	unsigned int m_count;

public:
	IndexBuffer(const unsigned int* indices, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;
};