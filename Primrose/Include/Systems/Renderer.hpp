#pragma once
#include "Utility.hpp"
#include "GraphicsResources.hpp"
#include "ShaderProgram.hpp" //Put in Graphics Resources!


class Core;
class Window;
class TextureStorage;
class ModelStorage;
class EntityComponentSystem;
class SpriteRenderer;


class Renderer final {
public:
	Renderer() = delete;
	explicit Renderer(Core& core);

	~Renderer();

	Renderer(const Renderer& other) = delete;
	Renderer& operator=(const Renderer& other) = delete;

	Renderer(Renderer&& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;

public:
	enum class RenderingView {
		LIT,
		UNLIT,
		DEPTH_ONLY,
		WIREFRAME
	};

public:
	[[nodiscard]] bool Update();

public:
	void CheckRendererAPIVersion() const;
	inline std::string GetLastExitMessage() noexcept { return m_LastExitMessage; };


private:
	void SetupShaderPrograms();


private:
	[[nodiscard]] bool Render2D();
	[[nodiscard]] bool Render3D();
	void SetupLightUniforms(ShaderProgram& program);


private:
	constexpr inline void RegisterExitMessage(std::string message) noexcept { m_LastExitMessage = message; };

private:
	void SetupMaterial(ShaderProgram& program, const SpriteRenderer* component);
	void UnbindAllTextures(const SpriteRenderer* component);

private:
	RenderingView m_CurrentRenderingView = RenderingView::LIT;

private:
	Shader m_DefaultLitVertex	{ GL_VERTEX_SHADER, "Resources/Shaders/DefaultLitVertex.glsl" };
	Shader m_DefaultLitFragment		{ GL_FRAGMENT_SHADER, "Resources/Shaders/DefaultLitFrag.glsl" };
	//UNLIT
	Shader m_DepthViewVertex	{ GL_VERTEX_SHADER, "Resources/Shaders/DepthViewVertex.glsl" };
	Shader m_DepthViewFragment	{ GL_FRAGMENT_SHADER, "Resources/Shaders/DepthViewFrag.glsl" };
	//WIREFRAME? NO SHADER?
	
private:
	ShaderProgram m_DepthViewShaderProgram;
	ShaderProgram m_DefaultLitShaderProgram;

private:
	std::string m_LastExitMessage;
	//Other diagnostics and warnings that need printing


private:
	Core* m_Core						{ nullptr };
	Window* m_Window					{ nullptr };
	TextureStorage* m_TextureStorage	{ nullptr };
	ModelStorage* m_ModelStorage		{ nullptr };
	EntityComponentSystem* m_ECS		{ nullptr };
};