#pragma once


enum class Keycode {

	Q = GLFW_KEY_Q,
	W = GLFW_KEY_W,
	E = GLFW_KEY_E,
	R = GLFW_KEY_R,
	T = GLFW_KEY_T,
	Y = GLFW_KEY_Y,
	U = GLFW_KEY_U,
	I = GLFW_KEY_I,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,

	A = GLFW_KEY_A,
	S = GLFW_KEY_S,
	D = GLFW_KEY_D,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,

	Z = GLFW_KEY_Z,
	X = GLFW_KEY_X,
	C = GLFW_KEY_C,
	V = GLFW_KEY_V,
	B = GLFW_KEY_B,
	N = GLFW_KEY_N,
	M = GLFW_KEY_M,

	NUM_0 = GLFW_KEY_0,
	NUM_1 = GLFW_KEY_1,
	NUM_2 = GLFW_KEY_2,
	NUM_3 = GLFW_KEY_3,
	NUM_4 = GLFW_KEY_4,
	NUM_5 = GLFW_KEY_5,
	NUM_6 = GLFW_KEY_6,
	NUM_7 = GLFW_KEY_7,
	NUM_8 = GLFW_KEY_8,
	NUM_9 = GLFW_KEY_9,

	F1 = GLFW_KEY_F1,
	F2 = GLFW_KEY_F2,
	F3 = GLFW_KEY_F3,
	F4 = GLFW_KEY_F4,
	F5 = GLFW_KEY_F5,
	F6 = GLFW_KEY_F6,
	F7 = GLFW_KEY_F7,
	F8 = GLFW_KEY_F8,
	F9 = GLFW_KEY_F9,
	F10 = GLFW_KEY_F10,
	F11 = GLFW_KEY_F11,
	F12 = GLFW_KEY_F12,

	DELETE = GLFW_KEY_DELETE,
	END = GLFW_KEY_END,
	HOME = GLFW_KEY_HOME,
	INSERT = GLFW_KEY_INSERT,
	PAGE_UP = GLFW_KEY_PAGE_UP,
	PAGE_DOWN = GLFW_KEY_PAGE_DOWN,

	UP = GLFW_KEY_UP,
	RIGHT = GLFW_KEY_RIGHT,
	LEFT = GLFW_KEY_LEFT,
	DOWN = GLFW_KEY_DOWN,

	COMMA = GLFW_KEY_COMMA,
	PERIOD = GLFW_KEY_PERIOD,
	MINUS = GLFW_KEY_MINUS,
	EQUAL = GLFW_KEY_EQUAL,
	APOSTROPHE = GLFW_KEY_APOSTROPHE,
	SLASH = GLFW_KEY_SLASH,
	BACKSLASH = GLFW_KEY_BACKSLASH,
	SEMICOLON = GLFW_KEY_SEMICOLON,

	TAB = GLFW_KEY_TAB,
	SPACE = GLFW_KEY_SPACE,
	ENTER = GLFW_KEY_ENTER,
	BACKSPACE = GLFW_KEY_BACKSPACE,
	LSHIFT = GLFW_KEY_LEFT_SHIFT,
	RSHIFT = GLFW_KEY_RIGHT_SHIFT,
	LCTRL = GLFW_KEY_LEFT_CONTROL,
	RCTRL = GLFW_KEY_RIGHT_CONTROL,
	LALT = GLFW_KEY_LEFT_ALT,
	RALT = GLFW_KEY_RIGHT_ALT,
	NUM_LOCK = GLFW_KEY_NUM_LOCK,
	CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
	MENU = GLFW_KEY_MENU,
	SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
	PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
	PAUSE = GLFW_KEY_PAUSE,
	RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
	LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,
	GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,
	RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
	LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
	ESCAPE = GLFW_KEY_ESCAPE
};
enum class MouseKeyCode {

	LEFT = GLFW_MOUSE_BUTTON_LEFT,
	RIGHT = GLFW_MOUSE_BUTTON_RIGHT,
	MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE,

	BUTTON_1 = GLFW_MOUSE_BUTTON_1,
	BUTTON_2 = GLFW_MOUSE_BUTTON_2,
	BUTTON_3 = GLFW_MOUSE_BUTTON_3,
	BUTTON_4 = GLFW_MOUSE_BUTTON_4,
	BUTTON_5 = GLFW_MOUSE_BUTTON_5,
	BUTTON_6 = GLFW_MOUSE_BUTTON_6,
	BUTTON_7 = GLFW_MOUSE_BUTTON_7,
	BUTTON_8 = GLFW_MOUSE_BUTTON_8,
};
enum class MouseMode {

	NORMAL = GLFW_CURSOR_NORMAL,
	HIDDEN = GLFW_CURSOR_HIDDEN,
	DISABLED = GLFW_CURSOR_DISABLED
};


namespace ScrollCapture {

	static double ScrollX;
	static double ScrollY;
}
namespace CursorCapture {

	static double CursorX;
	static double CursorY;
}


class Inputinator final {
public:
	Inputinator() = delete;
	Inputinator(GLFWwindow& window) noexcept
		:	m_WindowReference(&window)
	{
		auto ScrollCapture = [](GLFWwindow* window, double xoffset, double yoffset) {

			ScrollCapture::ScrollX = xoffset;
			ScrollCapture::ScrollY = yoffset;
		};
		glfwSetScrollCallback(m_WindowReference, ScrollCapture);

		auto CursorCapture = [](GLFWwindow* window, double xoffset, double yoffset) {

			CursorCapture::CursorX = xoffset;
			CursorCapture::CursorY = yoffset;
		};
		glfwSetCursorPosCallback(m_WindowReference, CursorCapture);
		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(m_WindowReference, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}

		SetMouseInputMode(m_CurrentMouseMode);
	};


public:
	[[nodiscard]] bool Update() {

		//TODO: move to window probably or something. Its a bit weird here
		if (GetKey(Keycode::ESCAPE)) {
			glfwSetWindowShouldClose(m_WindowReference, true);
		}

		return true;
	}

public:
	bool GetKey(Keycode key) const noexcept {
		return glfwGetKey(m_WindowReference, static_cast<int>(key));
	}
	bool GetMouseKey(MouseKeyCode key) const noexcept {
		return glfwGetMouseButton(m_WindowReference, static_cast<int>(key));
	}
	void SetMouseInputMode(MouseMode mode) noexcept {

		if (m_CurrentMouseMode != MouseMode::HIDDEN && mode == MouseMode::HIDDEN) {
			m_LastCursorPositionX = CursorCapture::CursorX;
			m_LastCursorPositionY = CursorCapture::CursorY;
		}

		glfwSetInputMode(m_WindowReference, GLFW_CURSOR, static_cast<int>(mode));
	}

	inline std::string GetLastExitMessage() const noexcept { return m_LastExitMessage; }

public:
	Vector2f GetMouseCursorDelta() noexcept { 
		
		Vector2f Results 
			= { static_cast<float>(CursorCapture::CursorX - m_LastCursorPositionX), static_cast<float>(m_LastCursorPositionY - CursorCapture::CursorY) };

		m_LastCursorPositionX = CursorCapture::CursorX;
		m_LastCursorPositionY = CursorCapture::CursorY;

		return Results;
	}
	float GetScrollDelta() noexcept {
		const float Delta = static_cast<float>(ScrollCapture::ScrollY);
		ScrollCapture::ScrollY = 0; //Reset it for next frame
		return Delta;
	}

private:
	inline void RegisterExitMessage(std::string message) noexcept { m_LastExitMessage = message; }

private:
	double m_LastCursorPositionX;
	double m_LastCursorPositionY;

private:
	std::string m_LastExitMessage;
	MouseMode m_CurrentMouseMode = MouseMode::NORMAL;
	GLFWwindow* m_WindowReference;
};