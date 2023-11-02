#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <exception>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define ASSERT(x) if (!(x)) __debugbreak();
#ifdef _DEBUG

#define GLCallVoid(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#define GLCall(x) [&](){\
    GLClearError();\
    auto retval = x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))\
    return retval;\
    }()
#else

#define GlCallVoid(x) x
#define GlCall(x) x

#endif

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char *function, const char *file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "OpenGL Error (" << error << ") > " << function << " [" << file << ":" << line << "]" << "\n";
        return false;
    }
    return true;
}

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

static unsigned int CreateShader(const std::string& vertex_shader, const std::string& fragment_shader)
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
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertex_shader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragment_shader);

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

    glfwSwapInterval(1);

    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        std::cout << "Error: " << glewGetErrorString(error) << "\n";
        return -1;
    }
    std::cout << "GLEW VERSION = " << glewGetString(GLEW_VERSION) << "\n";

    float positions[] = {
        -0.5f,  -0.5f, //0
         0.5f,  -0.5f, //1
         0.5f,   0.5f, //2
        -0.5f,   0.5f, //3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    // This is the index of the buffer of data that we will create
    unsigned int buffer_id;

    // Generate an internal buffer and assign an index to it
    GLCallVoid(glGenBuffers(1, &buffer_id));

    // Select the kind of buffer. In this case, an array of memory
    GLCallVoid(glBindBuffer(GL_ARRAY_BUFFER, buffer_id));

    // Create the actual buffer of data, specifying at least its size
    GLCallVoid(glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW)); // Can't resize this to 4 bytes, maybe it needs to store the final ammount of vertices to be drawn

    // Set the specification of the attribute "vertex position" in the vertex for the buffer
    GLCallVoid(glEnableVertexAttribArray(0));
    GLCallVoid(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)0));

    // Index buffer object
    unsigned int ibo;
    GLCallVoid(glGenBuffers(1, &ibo));
    GLCallVoid(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    GLCallVoid(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

    ShaderSource shader_source = ParseShaderSource("res/shaders/Basic.shader");

    std::cout << "VERTEX\n";
    std::cout << shader_source.VertexSource << '\n';
    std::cout << "FRAGMENT\n";
    std::cout << shader_source.FragmentSource << '\n';

    unsigned int shader_id = CreateShader(shader_source.VertexSource, shader_source.FragmentSource);
    GLCallVoid(glUseProgram(shader_id));

    int location = GLCall(glGetUniformLocation(shader_id, "u_Color"));
    ASSERT(location != -1); //if something has gone wrong or if the uniform has not been used in the shader (striped out in compilation)
    GLCallVoid(glUniform4f(location, 0.3f, 0.5f, 0.8f, 1.0f));

    float r_channel = 0.0f;
    float g_channel = 0.0f;
    float increment = 0.05f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCallVoid(glClear(GL_COLOR_BUFFER_BIT));

        // Draw shape
        GLCallVoid(glUniform4f(location, r_channel, g_channel, 0.8f, 1.0f));
        GLCallVoid(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr)); // nullptr here because the buffer is already bound to ibo

        // Animate the color of the shape
        if (r_channel > 1.0f)
        {
            increment = -0.005f;
        }
        else if (r_channel < 0.0f)
        {
            increment = 0.005f;
        }
        r_channel += increment;
        g_channel += increment;

        // Draw rectangle
        /*glBegin(GL_QUADS);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);*/

        /* Swap front and back buffers */
        GLCallVoid(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCallVoid(glfwPollEvents());
    }

    glDeleteProgram(shader_id);

    glfwTerminate();
    return 0;
}