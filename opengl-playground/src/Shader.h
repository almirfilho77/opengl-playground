#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

struct ShaderSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	unsigned int m_rendererId;
	std::unordered_map<std::string, int> m_uniformLocationMap;

public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
	ShaderSource m_ParseShaderSource(const std::string& filpath);
	unsigned int m_CreateShader(const std::string& vertex_shader, const std::string& fragment_shader);
	unsigned int m_CompileShader(unsigned int type, const std::string& source);

	int m_GetUniformLocation(const std::string& name);
};