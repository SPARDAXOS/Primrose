#pragma once
#include <GLAD/glad/glad.h>
#include <GLFW/glfw3.h>
#include <CppCoreCheck/warnings.h>
#include <string>
#include <stdexcept>
#include <memory>
#include "Utility.hpp"
#include <GraphicsResources.hpp>



class Window final {
public:
	Window() noexcept;

private:
	struct WindowResource {
		explicit WindowResource(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
			m_ptr = glfwCreateWindow(width, height, title, monitor, share);
			if (m_ptr == nullptr) {
				throw std::runtime_error(std::string("Failed to create window " + glfwGetError(nullptr)));
			}
			//TODO: check if window was created corrected and throw exception if not
		}
		~WindowResource() {
			glfwDestroyWindow(m_ptr);
		}

		WindowResource() = delete;
		WindowResource(const WindowResource&) = delete;
		WindowResource(WindowResource&&) = delete;
		WindowResource& operator=(const WindowResource&) = delete;
		WindowResource& operator=(WindowResource&&) = delete;


		GLFWwindow* m_ptr = nullptr;
	};
	struct GLFWResource {
		explicit GLFWResource() {
			if (glfwInit() != GLFW_TRUE) {
				throw std::runtime_error("GLFW failed to initialize");
			}
		}
		~GLFWResource() {
			glfwTerminate();
		}
	};
	struct GLADResource {
		explicit GLADResource() {
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
				throw std::runtime_error("Failed to initialize GLAD");
			}
		}
		~GLADResource() {

		}
	};

public:
	[[nodiscard]] bool UpdateWindow() noexcept;


private:
	void CreateWindow();
	void SetupGLFW();
	void SetupOpenGLFlags() noexcept;
	void SetupGLAD();
	void SetupViewport() noexcept;

private:
	void ProcessInput();

private:
	void Clear() noexcept;

private:
	std::unique_ptr<GLFWResource> m_GLFW;
	std::unique_ptr<GLADResource> m_GLAD;
	std::unique_ptr<WindowResource> m_Window;
};