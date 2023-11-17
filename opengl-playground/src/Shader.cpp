#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "GL/glew.h"

#include "Renderer.h"

Shader::Shader(const std::string& filepath)
{
    ShaderSource shader_source = m_ParseShaderSource(filepath);

    std::cout << "VERTEX\n";
    std::cout << shader_source.VertexSource << '\n';
    std::cout << "FRAGMENT\n";
    std::cout << shader_source.FragmentSource << '\n';

    m_rendererId = m_CreateShader(shader_source.VertexSource, shader_source.FragmentSource);
    GLCallVoid(glUseProgram(m_rendererId));
}

Shader::~Shader()
{
    GLCallVoid(glDeleteProgram(m_rendererId));
}

void Shader::Bind() const
{
    GLCallVoid(glUseProgram(m_rendererId));
}

void Shader::Unbind() const
{
    GLCallVoid(glUseProgram(m_rendererId));
}

ShaderSource Shader::m_ParseShaderSource(const std::string& filepath)
{
    std::fstream shader_file_stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1,
    };

    std::stringstream ss[2];
    ShaderType shader_type = ShaderType::NONE;

    std::string line;
    while (getline(shader_file_stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            // If we turn the following behavior into a function we can write unit tests 
            // to verify if the shader type is being correctly set
            if (line.find("vertex") != std::string::npos)
            {
                shader_type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                shader_type = ShaderType::FRAGMENT;
            }
        }
        else if (shader_type != ShaderType::NONE)
        {
            ss[(int)shader_type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::m_CreateShader(const std::string& vertex_shader, const std::string& fragment_shader)
{
    /*
    *   Create a program in OpenGL is like generate a buffer,
    *   it simply gives back an ID for it, but it does not init
    *   anything.
    */
    unsigned int program_id = GLCall(glCreateProgram());

    /*
    *   Compile the shaders
    */
    unsigned int vs = m_CompileShader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs = m_CompileShader(GL_FRAGMENT_SHADER, fragment_shader);

    /*
    *   Attach the shaders, meaning to specify what is going to be linked together
    */
    GLCallVoid(glAttachShader(program_id, vs));
    GLCallVoid(glAttachShader(program_id, fs));

    /*
    * Actually link the program
    */
    GLCallVoid(glLinkProgram(program_id));
    GLCallVoid(glValidateProgram(program_id));

    int validation_sucess = 0;
    GLCallVoid(glGetProgramiv(program_id, GL_VALIDATE_STATUS, &validation_sucess));

    return program_id;
}

unsigned int Shader::m_CompileShader(unsigned int type, const std::string& source)
{
    unsigned int shader_id = GLCall(glCreateShader(type));
    const char* src = source.c_str();
    GLCallVoid(glShaderSource(shader_id, 1, &src, nullptr));
    GLCallVoid(glCompileShader(shader_id));

    int result;
    GLCallVoid(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int log_length;
        GLCallVoid(glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length));
        char* error_message = (char*)alloca(log_length * sizeof(char)); // using alloca function to dynamically allocate this string on the stack (might stack overflow?)
        GLCallVoid(glGetShaderInfoLog(shader_id, log_length, &log_length, error_message));

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader") << "\n";
        std::cout << "Error message: " << error_message << "\n";
        GLCallVoid(glDeleteShader(shader_id));
        return 0;
    }

    return shader_id;
}

int Shader::m_GetUniformLocation(const std::string& name)
{
    auto search_retval = m_uniformLocationMap.find(name);
    if (search_retval != m_uniformLocationMap.end() &&
        search_retval->second != -1)
    {
        return m_uniformLocationMap[name];
    }

    int location = GLCall(glGetUniformLocation(m_rendererId, name.c_str()));
    if (location == -1)
    {
        std::cout << "Something is wrong with the uniform " << name << ".Maybe it was not initialized ? \n";
    }

    m_uniformLocationMap[name] = location;
    return location;
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    int location = m_GetUniformLocation(name);
    GLCallVoid(glUniform1i(location, value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    int location = m_GetUniformLocation(name);
    GLCallVoid(glUniform4f(location, v0, v1, v2, v3));
}

