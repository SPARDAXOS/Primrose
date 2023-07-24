#include <Window.hpp>
#include <iostream>

bool Window::Update() noexcept {

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    const bool State = glfwWindowShouldClose(m_Window->m_Handle); //Isnt this also in the input class?
    if (!State) {


        glfwPollEvents(); //TODO: Move this out to be put more clearly in the lower level loop
        return true;
    }
    else {
        //Clean up window
        RegisterExitMessage("Window closed");
        glfwTerminate();
        return false;
    }
}


void Window::SetWindowMode(WindowMode mode) noexcept {
    switch (mode) {
    case WindowMode::WINDOWED: {
        glfwSetWindowMonitor(m_Window->m_Handle, nullptr, 0, 0, m_Width, m_Height, m_RefreshRate);
    }break;
    case WindowMode::FULLSCREEN: {
        glfwSetWindowMonitor(m_Window->m_Handle, m_Window->m_Monitor, 0, 0, m_Width, m_Height, m_RefreshRate);
    }break;
    case WindowMode::BORDERLESS_FULLSCREEN: {

    }break;
    }
}
void Window::ClearBuffer() const noexcept {
    const Color ClearColor = Colors::PaleGreen;
    GLCall(glClearColor(ClearColor.m_R, ClearColor.m_G, ClearColor.m_B, ClearColor.m_A));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}
void Window::SwapBuffer() const noexcept {
    glfwSwapBuffers(m_Window->m_Handle);
}
void Window::BindOpenGLContext() const noexcept {
    glfwMakeContextCurrent(m_Window->m_Handle);
}
void Window::UnbindOpenGLContext() const noexcept {
    glfwMakeContextCurrent(nullptr);
}


void Window::CreateWindow() {
    m_Window = std::make_unique<WindowResource>(m_Width, m_Height, "Primrose", glfwGetPrimaryMonitor(), nullptr); //Uses main monitor
    if (m_Window->m_Handle == nullptr) {
        glfwTerminate();
        throw std::runtime_error("GLFW failed to create window");
    }
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
    GLCall(glViewport(0, 0, m_Width, m_Height)); 
    auto FrameBufferSizeCallback = [](GLFWwindow* window, int width, int height) { GLCall(glViewport(0, 0, width, height)); };
    glfwGetWindowSize(m_Window->m_Handle, &m_Width, &m_Height);
    glfwSetFramebufferSizeCallback(m_Window->m_Handle, FrameBufferSizeCallback);
    
}


void Window::UpdateWindowSize() noexcept {
    GLCall(glViewport(0, 0, m_Width, m_Height));
}