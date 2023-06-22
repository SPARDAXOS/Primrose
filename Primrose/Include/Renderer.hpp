#pragma once
#include "Utility.hpp"
#include "GraphicsResources.hpp"



class Window;

class Renderer final {
public:
	Renderer() = delete;
	Renderer(Window& ref) noexcept {
		m_WindowReference = &ref;


	};

public:
	void Render() const;
	void TestRender(const VAO& vao) const;


private:
	void Clear() const noexcept;


private:
	const Window* m_WindowReference;
};