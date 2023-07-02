#pragma once


//IMPORTANT: put on hold for now. Use GLFW instead.

enum class Keycode {
	
};

namespace ScrollCapture {

	static double ScrollX;
	static double ScrollY;

}


class Inputinator final {
public:
	Inputinator() = delete;
	Inputinator(GLFWwindow& window) 
		:	m_WindowReference(&window)
	{
		auto Lambda = [](GLFWwindow* window, double xoffset, double yoffset) {

			ScrollCapture::ScrollX = xoffset;
			ScrollCapture::ScrollY = yoffset;
		};

		glfwSetScrollCallback(m_WindowReference, Lambda);
		glfwSetInputMode(m_WindowReference, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	};


public:
	bool GetKey(Keycode key) const noexcept {
		return glfwGetKey(m_WindowReference, static_cast<int>(key));
	}


private:
	GLFWwindow* m_WindowReference;
};