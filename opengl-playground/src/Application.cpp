#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct ShaderSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderSource ParseShaderSource(const std::string& filepath)
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

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int shader_id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader_id, 1, &src, nullptr);
    glCompileShader(shader_id);

    int result;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int log_length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
        char* error_message = (char*)alloca(log_length * sizeof(char)); // using alloca function to dynamically allocate this string on the stack (might stack overflow?)
        glGetShaderInfoLog(shader_id, log_length, &log_length, error_message);

        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader") << "\n";
        std::cout << "Error message: " << error_message << "\n";
        glDeleteShader(shader_id);
        return 0;
    }

    return shader_id;
}

static unsigned int CreateShader(const std::string& vertex_shader, const std::string& fragment_shader)
{
    /*
    *   Create a program in OpenGL is like generate a buffer,
    *   it simply gives back an ID for it, but it does not init
    *   anything.
    */
    unsigned int program_id = glCreateProgram();

    /*
    *   Compile the shaders
    */
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragment_shader);

    /*
    *   Attach the shaders, meaning to specify what is going to be linked together 
    */
    glAttachShader(program_id, vs);
    glAttachShader(program_id, fs);

    /*
    * Actually link the program
    */
    glLinkProgram(program_id);
    glValidateProgram(program_id);

    int validation_sucess = 0;
    glGetProgramiv(program_id, GL_VALIDATE_STATUS, &validation_sucess);

    return program_id;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

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

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cout << "Error: " << glewGetErrorString(error) << "\n";
        return -1;
    }
    std::cout << "GLEW VERSION = " << glewGetString(GLEW_VERSION) << "\n";

    float positions[6] = {
        -0.5f,  -0.5f,
         0.5f,  -0.5f,
         0.0f,   0.5f,
    };

    // This is the index of the buffer of data that we will create
    unsigned int buffer_id;

    // Generate an internal buffer and assign an index to it
    glGenBuffers(1, &buffer_id);

    // Select the kind of buffer. In this case, an array of memory
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

    // Create the actual buffer of data, specifying at least its size
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

    // Set the specification of the attribute "vertex position" in the vertex for the buffer
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0);

    ShaderSource shader_source = ParseShaderSource("res/shaders/Basic.shader");

    std::cout << "VERTEX\n";
    std::cout << shader_source.VertexSource << '\n';
    std::cout << "FRAGMENT\n";
    std::cout << shader_source.FragmentSource << '\n';

    unsigned int shader_id = CreateShader(shader_source.VertexSource, shader_source.FragmentSource);
    glUseProgram(shader_id);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Draw rectangle
        /*glBegin(GL_QUADS);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);*/

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader_id);

    glfwTerminate();
    return 0;
}