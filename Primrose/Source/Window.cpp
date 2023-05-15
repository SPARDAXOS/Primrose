#include <Window.hpp>
#include <iostream>



Window::Window() noexcept {

    SetupGLFW();
    SetupOpenGLFlags();
    CreateWindow();
    SetupGLAD();
    SetupViewport();
    PrintMessage(glGetString(GL_VERSION));
}


bool Window::UpdateWindow() noexcept {
    while (!glfwWindowShouldClose(m_Window.get()->m_ptr)) {
        Clear();



        glfwSwapBuffers(m_Window->m_ptr);
        glfwPollEvents();
        return true;
    }

    glfwTerminate(); //Delete this from here?

    return false;
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
    glViewport(0, 0, 800, 600);
    auto FrameBufferSizeCallback = [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); };
    glfwSetFramebufferSizeCallback(m_Window->m_ptr, FrameBufferSizeCallback);
}