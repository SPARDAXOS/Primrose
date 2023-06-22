#pragma once
#include <GLAD/glad/glad.h>
#include "Utility.hpp"

#define GLCall(x) ClearGLErrors(); x; CheckGLError(#x, __FILE__, __LINE__);

static void ClearGLErrors() noexcept {

    while (glGetError() != GL_NO_ERROR);
}
static void CheckGLError(const char* function, const char* file, int line) noexcept {

    GLenum Error = glGetError();
    while (Error != GL_NO_ERROR) {
        PrintGLError(Error, file, function, line);
        Error = glGetError();
    }
}