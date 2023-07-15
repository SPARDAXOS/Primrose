#pragma once
#include <stdexcept>
#include <cstdio>
#include <memory>
#include <thread>

#include "Renderer.hpp"
#include "Window.hpp"
#include "FileManagment.hpp"
#include "ShaderCompiler.hpp"
#include "TextureStorage.hpp"
#include "EntityComponentSystem.hpp"
#include "GameObject.hpp"
#include "Time.hpp"
#include "Inputinator.hpp"
#include "Editor.hpp"


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
	explicit Core() noexcept;

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

	//TODO: move all input code to some other class
	void UpdateViewportControls();

	//TODO: make function in inputinator that retunrs differens betwenn last 2 frames
	double m_LastCursorPositionX;
	double m_LastCursorPositionY;

	float m_FreeLookSensitivity = 0.1f;
	float m_FreeLookSpeed = 10.0f;

	float m_CameraMovementSpeed = 5.0f;
	float m_CameraSpeedMax = 10.0f;
	float m_CameraSpeedMin = 1.0f;
	float m_CameraSpeedDecrease = 20.0f;
	float m_CameraSpeedIncrease = 20.0f;

	bool m_ViewportNavigationMode = false;

	int32 m_ViewportWidth = 1920;
	int32 m_ViewportHeight = 1080;

private:
	std::string m_ExitMessage;

private:
	bool m_Running = false;

public:
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<Window> m_Window;
	std::unique_ptr<TextureStorage> m_TextureStorage;
	std::unique_ptr<EntityComponentSystem> m_ECS;
	std::unique_ptr<Time> m_Time;
	std::unique_ptr<Inputinator> m_Input;
	std::unique_ptr<Editor> m_Editor;
};