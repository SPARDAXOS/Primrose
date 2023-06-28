#pragma once
#include "Utility.hpp"
#include "GraphicsResources.hpp"
#include "ShaderProgram.hpp"



class Window;
class EntityComponentSystem;
class GameObject;

class Renderer final {
public:
	Renderer() = delete;
	Renderer(EntityComponentSystem& ecs, Window& window) noexcept
		: m_WindowReference(&window), m_ECSReference(&ecs)
	{
	};

public:
	[[nodiscard]] bool Update() const;
	void TestRender(const VAO& vao) const;

public:
	inline std::string GetLastExitMessage() noexcept { return m_LastExitMessage; };

private:
	[[nodiscard]] bool Render2D() const;
	[[nodiscard]] bool Render3D() const;

private:
	void Clear() const noexcept;
	void SwapBuffers() const;

private:
	inline void RegisterExitMessage(std::string message) noexcept { m_LastExitMessage = message; };


private:
	std::string m_LastExitMessage;

private:
	Window* m_WindowReference;
	EntityComponentSystem* m_ECSReference;
};