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
	explicit Window();

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
	//[[nodiscard]] inline const GLFWwindow& GetWindowResource() const noexcept { return *m_Window.get()->m_ptr; }
	void SwapBuffers() const;

public:
	inline std::string GetLastExitMessage() noexcept { return m_LastExitMessage; };


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
	inline void RegisterExitMessage(std::string message) noexcept { m_LastExitMessage = message; };

private:
	void LoadShaders();
	[[nodiscard]] GLuint CreateShaderProgram();
	[[nodiscard]] bool LinkToShaderProgram(const GLuint& program, const GLuint& shader);
	[[nodiscard]] bool LinkShaderProgram(const GLuint& program);
	void UseShaderProgram(const GLuint& program);

	[[nodiscard]] bool CompileShader(GLenum type, const std::string_view& sourcecode, GLuint& ID);
	//[[nodiscard]] GLuint CreateShaderProgram(); //How many and which types are they!


private:
	bool m_VSync = true;

private:
	std::string_view m_VertexShaderFilePath;
	std::string_view m_FragmentShaderFilePath;

private:
	std::string m_VertexShaderSource;
	std::string m_FragmentShaderSource;


private:
	GLuint m_VertexShader;
	GLuint m_FragmentShader;

private:
	GLuint m_DefaultShaderProgram;

private:
	std::string m_LastExitMessage;

private:
	std::unique_ptr<GLFWResource> m_GLFW;
	std::unique_ptr<GLADResource> m_GLAD;
	std::unique_ptr<WindowResource> m_Window;
};