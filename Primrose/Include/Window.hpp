#pragma once
#include <GLAD/glad/glad.h>
#include <GLFW/glfw3.h>
#include <CppCoreCheck/warnings.h>
#include <stdexcept>
#include <memory>
#include "Utility.hpp"



class Window final {
public:
	Window();

	struct WindowWrapper {
		WindowWrapper(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
			m_Window = glfwCreateWindow(width, height, title, monitor, share);
		}
		~WindowWrapper() {
			glfwDestroyWindow(m_Window);
		}
		GLFWwindow* m_Window = nullptr;
	};

public:

public:
	[[nodiscard]] bool UpdateWindow() const noexcept;

private:
	void SetupGLFW();
	void SetupGLAD();

public:
	void CreateWindow();

public:
	std::unique_ptr<WindowWrapper> m_Window;
};