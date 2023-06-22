#include <Window.hpp>
#include <iostream>



Window::Window() {


    SetupGLFW();
    SetupOpenGLFlags();
    CreateWindow();
    SetupGLAD();
    SetupViewport();
    PrintMessage(glGetString(GL_VERSION));



    m_VertexShaderFilePath = "Resources/Shaders/Vertex.txt";
    m_FragmentShaderFilePath = "Resources/Shaders/Fragment.txt";

    LoadShaders();

    if (!CompileShader(GL_VERTEX_SHADER, m_VertexShaderSource, m_VertexShader)) {
        PrintMessage("Vertex shader failed to compile!");
    }
    if (!CompileShader(GL_FRAGMENT_SHADER, m_FragmentShaderSource, m_FragmentShader)) {
        PrintMessage("Fragment shader failed to compile!");
    }

    m_DefaultShaderProgram = CreateShaderProgram();
    if (!LinkToShaderProgram(m_DefaultShaderProgram, m_VertexShader)) {
        PrintMessage("Vertex shader failed to link to program!");
    }
    if (!LinkToShaderProgram(m_DefaultShaderProgram, m_FragmentShader)) {
        PrintMessage("Fragment shader failed to link to program!");
    }

    if (!LinkShaderProgram(m_DefaultShaderProgram)) {
        PrintMessage("Failed to link program!");
    }
    UseShaderProgram(m_DefaultShaderProgram);
}

void Window::LoadShaders() {
    //TODO: Create some wrapper for shaders

    if (!FileManagement::CRead(m_VertexShaderFilePath, m_VertexShaderSource)) {
        PrintMessage("Vertex shader source failed to load!");
    }
    if (!FileManagement::CRead(m_FragmentShaderFilePath, m_FragmentShaderSource)) {
        PrintMessage("Fragment shader source failed to load!");
    }
}
GLuint Window::CreateShaderProgram() {
    const auto Program = glCreateProgram();
    CheckGLError("CreateShaderProgram", __FILE__, __LINE__);
    return Program;
}
bool Window::LinkToShaderProgram(const GLuint& program, const GLuint& shader) {
    GLCall(glAttachShader(program, shader));
    GLCall(glDeleteShader(shader));
    return true;
}
bool Window::LinkShaderProgram(const GLuint& program) {
    GLCall(glLinkProgram(program));

    int LinkResults;
    glGetShaderiv(program, GL_LINK_STATUS, &LinkResults);
    if (LinkResults == GL_FALSE) {
        char ErrorMessage[512];
        glGetProgramInfoLog(program, 512, nullptr, ErrorMessage);
        PrintMessage(ErrorMessage);

        //glDeleteShader(ID);
        return false;
    }



    //glValidateProgram(program);
    return true;
}
void Window::UseShaderProgram(const GLuint& program) {
    GLCall(glUseProgram(program));
}

bool Window::CompileShader(GLenum type, const std::string_view& source, GLuint& ID) {

    ID = glCreateShader(type);
    CheckGLError("CompileShader", __FILE__, __LINE__);
    const GLchar* PointerToSource = source.data();
    GLCall(glShaderSource(ID, 1, &PointerToSource, nullptr));
    GLCall(glCompileShader(ID));


    //TODO: make this into a reusable function
    int CompilationResults;
    glGetShaderiv(ID, GL_COMPILE_STATUS, &CompilationResults);
    if (CompilationResults == GL_FALSE) {
        char ErrorMessage[512];
        glGetShaderInfoLog(ID, 512, nullptr, ErrorMessage);
        PrintMessage(ErrorMessage);

        glDeleteShader(ID);
        return false;
    }

    return true;
}




bool Window::UpdateWindow() noexcept {
    const Triangle Triangle;
    const Square Square;


    const VBO VertexBufferObject(Square.m_Data, sizeof(Square.m_Data));
    const EBO ElementBufferObject(Square.m_Indices, sizeof(Square.m_Indices));
    
    const VAO VertexArrayObject;

    VertexArrayObject.Bind();
    //glBindVertexArray(VertexArrayObject.m_ID);

    //Leaks VRAM like crazy
    //glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject.m_ID);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(Square.m_Data), &Square.m_Data, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject.m_ID);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Square.m_Indices), Square.m_Indices, GL_STATIC_DRAW);

    VertexBufferObject.Bind();
    ElementBufferObject.Bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glEnableVertexAttribArray(0);


    VertexArrayObject.Unbind();
    /*glBindVertexArray(0);*/

    VertexBufferObject.Unbind();
    ElementBufferObject.Unbind();
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    const bool State = glfwWindowShouldClose(m_Window->m_ptr);
    if (!State) {
        //Do window shit
        Clear();

        ProcessInput();

        //glBindVertexArray(VertexArrayObject.m_ID);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject.m_ID);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        VertexArrayObject.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(m_Window->m_ptr);
        glfwPollEvents();
        return true;
    }
    else {
        //Clean up window
        RegisterExitMessage("Window closed");
        glfwTerminate();
        glDeleteProgram(m_DefaultShaderProgram); //TODO: setup some RAII instead
        return false;
    }
}


void Window::ProcessInput() {
    if (glfwGetKey(m_Window->m_ptr, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(m_Window->m_ptr, true);
    }


}


void Window::Clear() noexcept {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


void Window::CreateWindow() {
    m_Window = std::make_unique<WindowResource>(800, 600, "Primrose", nullptr, nullptr);
    if (m_Window->m_ptr == nullptr) {
            glfwTerminate();
            throw std::runtime_error("GLFW failed to create window");
    }
    glfwMakeContextCurrent(m_Window->m_ptr);
}
void Window::SetupGLFW() {
    m_GLFW = std::make_unique<GLFWResource>();
}
void Window::SetupOpenGLFlags() noexcept {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
void Window::SetupGLAD() {
    m_GLAD = std::make_unique<GLADResource>();
}
void Window::SetupViewport() noexcept {
    GLCall(glViewport(0, 0, 800, 600));
    auto FrameBufferSizeCallback = [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); };
    glfwSetFramebufferSizeCallback(m_Window->m_ptr, FrameBufferSizeCallback);
}