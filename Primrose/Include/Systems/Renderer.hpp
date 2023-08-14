#pragma once
#include "Utility.hpp"
#include "GraphicsResources.hpp"
#include "ShaderProgram.hpp"


class Core;
class Window;
class TextureStorage;
class EntityComponentSystem;
class SpriteRenderer;


class Renderer final {
public:
	Renderer() = delete;
	Renderer(Core& core) noexcept;

public:
	[[nodiscard]] bool Update();

public:
	void CheckRendererAPIVersion();
	inline std::string GetLastExitMessage() noexcept { return m_LastExitMessage; };

private:
	[[nodiscard]] bool Render2D();
	[[nodiscard]] bool Render3D();

private:
	inline void RegisterExitMessage(std::string message) noexcept { m_LastExitMessage = message; };

private:
	void SetupMaterial(ShaderProgram& program, const SpriteRenderer* component);
	void UnbindAllTextures(const SpriteRenderer* component);

private:
	std::string m_LastExitMessage;

private:
	Core* m_Core							{ nullptr };
	Window* m_WindowReference					{ nullptr };
	TextureStorage* m_TextureStorageReference	{ nullptr };
	EntityComponentSystem* m_ECSReference		{ nullptr };
};