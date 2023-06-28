#pragma once
#include "Utility.hpp"
#include "GraphicsResources.hpp"



class Window;
class EntityComponentSystem;

class Renderer final {
public:
	Renderer() = delete;
	Renderer(EntityComponentSystem& ecs, Window& window) noexcept
		: m_WindowReference(&window), m_ECSReference(&ecs)
	{
	};

public:
	void Update() const;
	void TestRender(const VAO& vao) const;

private:
	void Render2D() const noexcept;
	void Render3D() const noexcept;

private:
	void Clear() const noexcept;
	void SwapBuffers() const;


private:
	const Window* m_WindowReference;
	const EntityComponentSystem* m_ECSReference;
};