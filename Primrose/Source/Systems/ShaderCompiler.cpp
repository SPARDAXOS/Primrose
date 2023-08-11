#include "Systems/ShaderCompiler.hpp"
#include "Systems/Serializer.hpp"


[[nodiscard]] bool ShaderCompiler::LoadShader(const std::string_view& filePath, std::string& buffer) const {
    //TODO: Create some wrapper for shaders

    return Serializer::CRead(filePath, buffer);
}
[[nodiscard]] bool ShaderCompiler::CompileShader(GLuint& ID, GLenum type, const std::string_view& source) const noexcept {
    ID = glCreateShader(type);
    CheckGLError("CompileShader", __FILE__, __LINE__);

    const GLchar* PointerToSource = source.data();
    GLCall(glShaderSource(ID, 1, &PointerToSource, nullptr));
    GLCall(glCompileShader(ID));

    return CheckCompileStatus(ID);
}


[[nodiscard]] bool ShaderCompiler::CreateShaderProgram(GLuint& ID) const noexcept { //Is this even needed anymore?
    ID = glCreateProgram();
    CheckGLError("CreateShaderProgram", __FILE__, __LINE__);
    return ID == 0 ? false : true;
}
[[nodiscard]] bool ShaderCompiler::LinkShaderProgram(const GLuint& program) const noexcept {
    GLCall(glLinkProgram(program));
    return CheckProgramLinkStatus(program);
}
void ShaderCompiler::UseShaderProgram(const GLuint& program) const noexcept {
    GLCall(glUseProgram(program));
}


void ShaderCompiler::AttachShader(const GLuint& shader, const GLuint& program) const noexcept {
    GLCall(glAttachShader(program, shader));
}
void ShaderCompiler::DetachShader(const GLuint& shader, const GLuint& program) const noexcept {
    glDetachShader(program, shader);
}
[[nodiscard]] bool ShaderCompiler::DeleteShader(GLuint& shader) const noexcept {
    /*
    If a shader object is deleted while it is attached to a program object, it will be flagged for deletion, 
    and deletion will not occur until glDetachShader is called to detach it from all program objects to which it is attached.
    */


    GLCall(glDeleteShader(shader));
    GLint DeletionStatus;
    GLCall(glGetShaderiv(shader, GL_DELETE_STATUS, &DeletionStatus));
    if (DeletionStatus == GL_TRUE) {
        shader = 0;
        return true;
    }
    return false;
}


[[nodiscard]] bool ShaderCompiler::CheckCompileStatus(const GLuint& ID) const noexcept {

    int CompilationResults;
    glGetShaderiv(ID, GL_COMPILE_STATUS, &CompilationResults);
    if (CompilationResults == GL_FALSE) {
        char ErrorMessage[512];
        glGetShaderInfoLog(ID, 512, nullptr, ErrorMessage);
        PrintMessage(ErrorMessage);

        //glDeleteShader(ID);
        return false;
    }

    return true;
}
[[nodiscard]] bool ShaderCompiler::CheckProgramLinkStatus(const GLuint& ID) const noexcept {

    GLint ProgramLinkResults;
    GLCall(glGetProgramiv(ID, GL_VALIDATE_STATUS, &ProgramLinkResults));
    if (ProgramLinkResults == GL_FALSE) {
        char ErrorMessage[512];
        glGetProgramInfoLog(ID, 512, nullptr, ErrorMessage);
        PrintMessage(ErrorMessage);
        return false;
    }

    GLCall(glValidateProgram(ID));
    return true;
}
[[nodiscard]] bool ShaderCompiler::CheckShaderLinkStatus(const GLuint& ID) const noexcept {

    //This does not work!
    GLint ShaderLinkResults;
    GLCall(glGetShaderiv(ID, GL_SHADER_TYPE, &ShaderLinkResults));
    if (ShaderLinkResults == GL_FALSE) {
        char ErrorMessage[512];
        glGetShaderInfoLog(ID, 512, nullptr, ErrorMessage);
        PrintMessage(ErrorMessage);
        return false;
    }

    return true;
}