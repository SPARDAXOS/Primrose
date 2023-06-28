#include <Window.hpp>
#include <iostream>



Window::Window() {


    SetupGLFW();
    SetupOpenGLFlags();
    CreateWindow();
    SetupGLAD();
    SetupViewport();
    PrintMessage(glGetString(GL_VERSION));

    glfwSwapInterval(m_VSync); //Vsync?

    m_VertexShaderFilePath = "Resources/Shaders/Vertex.txt";
    m_FragmentShaderFilePath = "Resources/Shaders/Fragment.txt";
}

void Window::SwapBuffers() const {
    glfwSwapBuffers(m_Window->m_ptr);
    //glfwPollEvents(); //There is a change in order now since this happens before the above! maybe
}



bool Window::Update() noexcept {

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    const bool State = glfwWindowShouldClose(m_Window->m_ptr);
    if (!State) {
        //Do window shit
        Clear();
        ProcessInput();

        //glfwSwapBuffers(m_Window->m_ptr);
        glfwPollEvents();
        return true;
    }
    else {
        //Clean up window
        RegisterExitMessage("Window closed");
        glfwTerminate();
        return false;
    }
}


void Window::ProcessInput() {
    if (glfwGetKey(m_Window->m_ptr, GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(m_Window->m_ptr, true);
    }


}


void Window::Clear() noexcept {
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
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