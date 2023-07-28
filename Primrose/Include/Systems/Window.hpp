#pragma once
#include <GLAD/glad/glad.h>
#include <GLFW/glfw3.h>
#include <CppCoreCheck/warnings.h>
#include <string>
#include <stdexcept>
#include <memory>
#include <GraphicsResources.hpp>

#include "Utility.hpp"



class Core;



enum class WindowMode {
	WINDOWED,
	FULLSCREEN,
	BORDERLESS_FULLSCREEN
};


class Window final {
public:
	Window() = delete;
	Window(Core& core);

private:
	struct WindowResource {
		explicit WindowResource(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {

			m_Handle = glfwCreateWindow(width, height, title, monitor, share);
			if (m_Handle == nullptr)
				throw std::runtime_error(std::string("Failed to create window " + glfwGetError(nullptr)));

			m_Monitor = monitor;

			m_VideoMode = glfwGetVideoMode(m_Monitor);
			if (m_VideoMode == nullptr)
				throw std::runtime_error(std::string("Failed to get video mode " + glfwGetError(nullptr)));
		}
		~WindowResource() {
			glfwDestroyWindow(m_Handle);
		}

		WindowResource() = delete;
		WindowResource(const WindowResource&) = delete;
		WindowResource(WindowResource&&) = delete;
		WindowResource& operator=(const WindowResource&) = delete;
		WindowResource& operator=(WindowResource&&) = delete;


		GLFWwindow* m_Handle = nullptr;
		GLFWmonitor* m_Monitor = nullptr;
		const GLFWvidmode* m_VideoMode = nullptr;
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
	[[nodiscard]] inline const GLFWwindow& GetWindowResource() const noexcept { return *m_Window.get()->m_Handle; }


public:
	inline std::string GetLastExitMessage() const noexcept { return m_LastExitMessage; }
	inline WindowResource& GetWindowResource() noexcept { return *m_Window.get(); }
	

public:
	void SetWindowMode(WindowMode mode) noexcept;
	void ClearBuffer() const noexcept;
	void SwapBuffer() const noexcept;
	void BindOpenGLContext() const noexcept;
	void UnbindOpenGLContext() const noexcept;


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
	int32 m_RefreshRate = 144;

private:
	std::string m_LastExitMessage;

private:
	Core* m_EngineCore	{ nullptr };
	std::unique_ptr<GLFWResource> m_GLFW;
	std::unique_ptr<GLADResource> m_GLAD;
	std::unique_ptr<WindowResource> m_Window;
};