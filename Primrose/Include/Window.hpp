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
	Window() = delete;
	Window(int32 width, int32 height)
		:	m_Width(width), m_Height(height)
	{
		SetupGLFW();
		SetupOpenGLFlags();
		CreateWindow();
		BindOpenGLContext();
		SetupGLAD();
		SetupViewport();
		PrintMessage(glGetString(GL_VERSION));

		glfwSwapInterval(m_VSync); //Vsync? Shouldnt even be a bool
	}

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
	[[nodiscard]] bool Update() noexcept;

public:
	inline void SetWidth(int32 width) noexcept { m_Width = width; UpdateWindowSize(); }
	inline void SetHeight(int32 height) noexcept {  m_Height = height; UpdateWindowSize(); }

	inline int32 GetWidth() const noexcept { return m_Width; }
	inline int32 GetHeight() const noexcept { return m_Height; }
	[[nodiscard]] inline const GLFWwindow& GetWindowResource() const noexcept { return *m_Window.get()->m_ptr; }


public:
	inline std::string GetLastExitMessage() const noexcept { return m_LastExitMessage; }
	inline WindowResource& GetWindowResource() noexcept { return *m_Window.get(); }
	

	void BindOpenGLContext() {
		glfwMakeContextCurrent(m_Window->m_ptr);
	}
	void UnbindOpenGLContext() {
		glfwMakeContextCurrent(nullptr);
	}


private:
	void CreateWindow();
	void SetupGLFW();
	void SetupOpenGLFlags() noexcept;
	void SetupGLAD();
	void SetupViewport() noexcept;

private:
	void UpdateWindowSize() noexcept;

private:
	inline void RegisterExitMessage(std::string message) noexcept { m_LastExitMessage = message; }

private:
	int32 m_Width;
	int32 m_Height;
	uint8 m_VSync = 1;

private:
	GLuint m_DefaultShaderProgram;

private:
	std::string m_LastExitMessage;

private:
	std::unique_ptr<GLFWResource> m_GLFW;
	std::unique_ptr<GLADResource> m_GLAD;
	std::unique_ptr<WindowResource> m_Window;
};