#pragma once
#include <GLAD/glad/glad.h>
#include "Utility.hpp"

#define GLCall(x) CheckGLError(); x;

static void ClearGLErrors() noexcept {

    while (glGetError() != GL_NO_ERROR);
}
static void CheckGLError() noexcept {

    GLenum Error = glGetError();
    while (Error != GL_NO_ERROR) {
        Error = glGetError();
        PrintGLError(Error);
    }
}