#include <Window.hpp>



Window::Window() {
    try {
        SetupGLFW();
        SetupGLAD();


    }
    catch (const std::exception& exception) {
        PrintMessage(exception.what());
    }
}


bool Window::UpdateWindow() const noexcept {
    while (!glfwWindowShouldClose(m_Window.get()->m_Window)) {
        glClearColor(0.2f, 0.6f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        glfwSwapBuffers(m_Window.get()->m_Window);
        glfwPollEvents();
        return true;
    }

    glfwTerminate();

    return false;
}


void Window::SetupGLFW() {
    try {
        if (glfwInit() != GLFW_TRUE) {
            throw std::runtime_error("GLFW failed to initialize");
        }
    }
    catch (const std::exception& exception) {
        PrintMessage(exception.what());
    }
}
void Window::SetupGLAD() {
    try {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD");
        }
    }
    catch (const std::exception& exception) {
        PrintMessage(exception.what());
    }
}

void Window::CreateWindow() {
    try {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //Cant Compile!
        m_Window = std::make_unique<WindowWrapper>(800, 600, "Primrose", nullptr, nullptr);
        if (m_Window == nullptr) {
            glfwTerminate();
            throw std::runtime_error("GLFW failed to create window");
        }
        glfwMakeContextCurrent(m_Window.get()->m_Window); //Horrible looking

        glViewport(0, 0, 800, 600);

        auto FrameBufferSizeCallback = [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); };

        glfwSetFramebufferSizeCallback(m_Window.get()->m_Window, FrameBufferSizeCallback);
    }
    catch (const std::exception& exception) {
        PrintMessage(exception.what());
    }
}