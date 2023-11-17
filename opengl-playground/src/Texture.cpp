#include "Texture.h"

#include <stb/stb_image.h>

#include "Renderer.h"

Texture::Texture(const std::string& filepath)
	:	m_rendererId(0),
		m_filePath(filepath),
		m_localBuffer(nullptr),
		m_width(0),
		m_height(0),
		m_bitsPerPixel(0)
{
	stbi_set_flip_vertically_on_load(1);
	m_localBuffer = stbi_load(filepath.c_str(), &m_width, &m_height, &m_bitsPerPixel, 4);

	GLCallVoid(glGenTextures(1, &m_rendererId));
	GLCallVoid(glBindTexture(GL_TEXTURE_2D, m_rendererId));

	GLCallVoid(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCallVoid(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCallVoid(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCallVoid(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCallVoid(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, 
		GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer));
	GLCallVoid(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_localBuffer)
	{
		stbi_image_free(m_localBuffer);
	}
}

Texture::~Texture()
{
	GLCallVoid(glDeleteTextures(1, &m_rendererId));
}

void Texture::Bind(unsigned int slot) const
{
	GLCallVoid(glActiveTexture(GL_TEXTURE0 + slot));
	GLCallVoid(glBindTexture(GL_TEXTURE_2D, m_rendererId));
}

void Texture::Unbind() const
{
	GLCallVoid(glBindTexture(GL_TEXTURE_2D, 0));
}
