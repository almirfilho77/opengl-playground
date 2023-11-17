#pragma once

#include <GL/glew.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

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

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    void Clear();
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader);
};