#include <Window.hpp>



Window::Window() {
    //try {
    //    SetupGLFW();
    //    SetupGLAD();


    //}
    //catch (const std::exception& exception) {
    //    PrintMessage(exception.what());
    //}
    CreateWindow();
}


bool Window::UpdateWindow() const noexcept {
    while (!glfwWindowShouldClose(m_Window.get()->m_ptr)) {
        glClearColor(0.2f, 0.6f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);



        glfwSwapBuffers(m_Window.get()->m_ptr);
        glfwPollEvents();
        return true;
    }

    glfwTerminate(); //Delete this from here?

    return false;
}


void Window::CreateWindow() {
    m_GLFW = std::make_unique<GLFWResource>();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    //Cant Compile!
    m_Window = std::make_unique<WindowResource>(800, 600, "Primrose", nullptr, nullptr);
    if (m_Window->m_ptr == nullptr) {
            glfwTerminate();
            throw std::runtime_error("GLFW failed to create window");
        }
    glfwMakeContextCurrent(m_Window->m_ptr); //Horrible looking

    m_GLAD = std::make_unique<GLADResource>();

    glViewport(0, 0, 800, 600);

    auto FrameBufferSizeCallback = [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); };

    glfwSetFramebufferSizeCallback(m_Window->m_ptr, FrameBufferSizeCallback);
   
}