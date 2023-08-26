#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class DetailsWindow;

class MainMenuBar final {
public:
	MainMenuBar() = delete;
	MainMenuBar(Core& core, Editor& editor) noexcept;
	~MainMenuBar() = default;

	MainMenuBar(const MainMenuBar&) = delete;
	MainMenuBar& operator=(const MainMenuBar&) = delete;

	MainMenuBar(MainMenuBar&&) = delete;
	MainMenuBar& operator=(MainMenuBar&&) = delete;

public:
	void Render();

public:
	ImVec2 GetSize() const noexcept { return m_Size; }

private:
	void RenderFileMenu();
	void RenderWindowMenu();
	void RenderAboutMenu();

private:
	ImVec2 m_Size;

private:
	Core* m_Core					{ nullptr };
	Editor* m_Editor				{ nullptr };
	ImGuiViewport* m_ImGuiViewport	{ nullptr };
	DetailsWindow* m_DetailsWindow	{ nullptr };
};