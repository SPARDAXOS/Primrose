#pragma once

#include <stdexcept>
#include <cstdio>
#include <memory>
#include <thread>
#include <iostream>

#include "Systems/Renderer.hpp"
#include "Systems/Window.hpp"
#include "Systems/AssetManager.hpp"
#include "Systems/ShaderCompiler.hpp"
#include "Systems/TextureStorage.hpp"
#include "Systems/EntityComponentSystem.hpp"
#include "GameObject.hpp" //??
#include "Systems/Time.hpp"
#include "Systems/Input.hpp"
#include "Systems/Editor.hpp"
#include "Systems/Serializer.hpp"
#include "Systems/Physics.hpp"
#include "Systems/ModelStorage.hpp"

#include "Tools/Logger.hpp"
#include "Utility.hpp" //Tools?




class Core final {
public:
	explicit Core() noexcept;

	void Run();
	void Exit();


public:
	[[nodiscard]] inline Renderer* GetRenderer() noexcept { return m_Renderer.get(); }
	[[nodiscard]] inline Window* GetWindow() noexcept { return m_Window.get(); }
	[[nodiscard]] inline TextureStorage* GetTextureStorage() noexcept { return m_TextureStorage.get(); }
	[[nodiscard]] inline EntityComponentSystem* GetECS() noexcept { return m_ECS.get(); }
	[[nodiscard]] inline AssetManager* GetAssetManager() noexcept { return m_AssetManager.get(); }
	[[nodiscard]] inline Time* GetTime() noexcept { return m_Time.get(); }
	[[nodiscard]] inline Input* GetInput() noexcept { return m_Input.get(); }
	[[nodiscard]] inline Editor* GetEditor() noexcept { return m_Editor.get(); }
	[[nodiscard]] inline Serializer* GetSerializer() noexcept { return m_Serializer.get(); }
	[[nodiscard]] inline Logger* GetLogger() noexcept { return m_Logger.get(); }
	[[nodiscard]] inline Physics* GetPhysics() noexcept { return m_Physics.get(); }
	[[nodiscard]] inline ModelStorage* GetModelStorage() noexcept { return m_ModelStorage.get(); }
	

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
	std::unique_ptr<ModelStorage> m_ModelStorage;
};