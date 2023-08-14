#pragma once

#include <stdexcept>
#include <cstdio>
#include <memory>
#include <thread>

#include "Renderer.hpp"
#include "Window.hpp"
#include "AssetManager.hpp"
#include "ShaderCompiler.hpp"
#include "TextureStorage.hpp"
#include "EntityComponentSystem.hpp"
#include "GameObject.hpp"
#include "Time.hpp"
#include "Input.hpp"
#include "Editor.hpp"
#include "Serializer.hpp"
#include "Physics.h"

#include "Tools/Logger.hpp"


#pragma warning(push, 0)
#include "STB_Image/stb_image.h"
#pragma warning(pop)

#include "Utility.hpp"


class Core final {
public:
	explicit Core() noexcept;

	void Run();
	void Exit();


public:
	[[nodiscard]] inline Renderer* GetRenderer() const noexcept { return m_Renderer.get(); }
	[[nodiscard]] inline Window* GetWindow() const noexcept { return m_Window.get(); }
	[[nodiscard]] inline TextureStorage* GetTextureStorage() const noexcept { return m_TextureStorage.get(); }
	[[nodiscard]] inline EntityComponentSystem* GetECS() const noexcept { return m_ECS.get(); }
	[[nodiscard]] inline AssetManager* GetAssetManager() const noexcept { return m_AssetManager.get(); }
	[[nodiscard]] inline Time* GetTime() const noexcept { return m_Time.get(); }
	[[nodiscard]] inline Input* GetInput() const noexcept { return m_Input.get(); }
	[[nodiscard]] inline Editor* GetEditor() const noexcept { return m_Editor.get(); }
	[[nodiscard]] inline Serializer* GetSerializer() const noexcept { return m_Serializer.get(); }
	[[nodiscard]] inline Logger* GetLogger() const noexcept { return m_Logger.get(); }
	[[nodiscard]] inline Physics* GetPhysics() const noexcept { return m_Physics.get(); }

public:
	void DebugLog(std::string message) noexcept;
	void WarningLog(std::string message) noexcept;
	void ErrorLog(std::string message) noexcept;
	void SystemLog(std::string message) noexcept;

public:
	inline uint32 GetWindowWidth() const noexcept { return m_WindowWidth; }
	inline uint32 GetWindowHeight() const noexcept { return m_WindowHeight; }

private:
	void SetupCore();
	void UpdateSystems();

private:
	void RegisterExitMessage(std::string message) noexcept;
	void PrintExitMessage() noexcept;

	int32 m_WindowWidth = 1920;
	int32 m_WindowHeight = 1080;

private:
	std::string m_ExitMessage;

private:
	bool m_Running = false;

private:
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<TextureStorage> m_TextureStorage;
	std::unique_ptr<EntityComponentSystem> m_ECS;
	std::unique_ptr<AssetManager> m_AssetManager;
	std::unique_ptr<Time> m_Time;
	std::unique_ptr<Input> m_Input;
	std::unique_ptr<Editor> m_Editor;
	std::unique_ptr<Serializer> m_Serializer;
	std::unique_ptr<Logger> m_Logger;
	std::unique_ptr<Physics> m_Physics;
};