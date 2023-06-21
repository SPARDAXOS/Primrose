#pragma once
#include <GLAD/glad/glad.h>
#include "Utility.hpp"



class Window;

class Renderer final {
public:
	Renderer() = delete;
	Renderer(Window& ref) noexcept {
		m_WindowReference = &ref;


	};

public:
	void Render() const;


private:
	void Clear() noexcept;


private:
	const Window* m_WindowReference;
};