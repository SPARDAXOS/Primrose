#pragma once
#include "Utility.hpp"
#include "GraphicsResources.hpp"
#include "ShaderProgram.hpp" //Put in Graphics Resources!


class Core;
class Window;
class Input;
class TextureStorage;
class ModelStorage;
class EntityComponentSystem;
class SpriteRenderer;
class SkeletalMesh;
class GameObject;
class Camera;


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
	void CheckRendererAPIVersion() const; //??? Why is this public
	inline std::string GetLastExitMessage() noexcept { return m_LastExitMessage; };


private:
	void SetupShaderPrograms();


private:
	[[nodiscard]] bool Render2D();
	[[nodiscard]] bool Render3D();
	void CheckInput() noexcept;




private:
	constexpr inline void RegisterExitMessage(std::string message) noexcept { m_LastExitMessage = message; };

private:
	void BindShaderProgram() const noexcept;
	void UnbindShaderProgram() const noexcept;
	void SetupMVP(Camera* viewport, glm::mat4* model);
	void SetupLightUniforms(ShaderProgram& program);
	void SetupMaterial(ShaderProgram& program, const SpriteRenderer* component);
	void UnbindAllTextures(const SpriteRenderer* component);

private:
	void Setup2DRenderingData() noexcept;

private:
	[[nodiscard]] bool Validate(const SpriteRenderer* component) const noexcept;
	[[nodiscard]] bool Validate(const SkeletalMesh* component) const noexcept;
	[[nodiscard]] bool Validate(const GameObject* object) const noexcept;

private:
	RenderingView m_CurrentRenderingView = RenderingView::LIT;

private:
	Shader m_DefaultLitVertex	{ GL_VERTEX_SHADER, "Resources/Shaders/DefaultLitVertex.glsl" };
	Shader m_DefaultLitFragment	{ GL_FRAGMENT_SHADER, "Resources/Shaders/DefaultLitFrag.glsl" };
	//UNLIT
	Shader m_DepthViewVertex	{ GL_VERTEX_SHADER, "Resources/Shaders/DepthViewVertex.glsl" };
	Shader m_DepthViewFragment	{ GL_FRAGMENT_SHADER, "Resources/Shaders/DepthViewFrag.glsl" };
	//WIREFRAME? NO SHADER?
	
private:
	ShaderProgram* m_CurrentShaderProgram	{nullptr};
	ShaderProgram m_DepthViewShaderProgram;
	ShaderProgram m_DefaultLitShaderProgram;

private:
	std::string m_LastExitMessage;
	//Other diagnostics and warnings that need printing

private:
	bool m_Render2DReady = false;
	Cube m_SpritePrimitive;
	VAO m_SpriteVAO;
	VBO m_SpriteVBO;
	EBO m_SpriteEBO;

private:
	Core* m_Core						{ nullptr };
	Window* m_Window					{ nullptr };
	Input* m_Input					{ nullptr };
	TextureStorage* m_TextureStorage	{ nullptr };
	ModelStorage* m_ModelStorage		{ nullptr };
	EntityComponentSystem* m_ECS		{ nullptr };
};