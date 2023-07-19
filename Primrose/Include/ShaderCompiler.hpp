#pragma once
#include "FileSystem.hpp"
#include <string_view>
#include "GLAD/glad/glad.h"
#include "GLErrorHandling.hpp"

class ShaderCompiler final {
public:
    [[nodiscard]] bool LoadShader(const std::string_view& filePath, std::string& buffer) const;
    [[nodiscard]] bool CompileShader(GLuint& ID, GLenum type, const std::string_view& source) const noexcept;

public:
    [[nodiscard]] bool CreateShaderProgram(GLuint& ID) const noexcept;
    [[nodiscard]] bool LinkShaderProgram(const GLuint& program) const noexcept;
    void UseShaderProgram(const GLuint& program) const noexcept; //Not sure about this yet!

public:
    void AttachShader(const GLuint& shader, const GLuint& program) const noexcept;
    void DetachShader(const GLuint& shader, const GLuint& program) const noexcept;
    [[nodiscard]] bool DeleteShader(GLuint& shader) const noexcept;


private:
    [[nodiscard]] bool CheckCompileStatus(const GLuint& ID) const noexcept;
    [[nodiscard]] bool CheckProgramLinkStatus(const GLuint& ID) const noexcept;
    [[nodiscard]] bool CheckShaderLinkStatus(const GLuint& ID) const noexcept;
};