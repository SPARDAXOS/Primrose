#pragma once
#include <stdexcept>
#include <cstdio>
#include <memory>

#include "Renderer.hpp"
#include "Window.hpp"
#include "FileManagment.hpp"
#include "ShaderCompiler.hpp"
#include "TextureStorage.hpp"
#include "EntityComponentSystem.hpp"

#pragma warning(push, 0)
#include "STB_Image/stb_image.h"
#pragma warning(pop)

#include "Utility.hpp"


/*
TODO: Implement RendererBackend
TODO: Implement Renderer
TODO: Implement TextureStorage
TODO: Implement ECS

//Test Comment
*/


class Core final {
public:
	explicit Core();

	void Run();
	void Exit();


public:
	[[nodiscard]] inline Window* GetWindow() const noexcept { return m_Window.get(); };

private:
	void SetupCore();


	//TODO: Rule of 6 - define special member functions 

	void UpdateSystems();

private:
	void RegisterExitMessage(std::string message) noexcept;
	void PrintExitMessage() noexcept;

private:
	std::string m_ExitMessage;

private:
	bool m_Running = false;

public:
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<TextureStorage> m_TextureStorage;
};