#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class DetailsWindow;
class HierarchyWindow;
class ContentBrowser;
class DebugLogWindow;
class SystemLogWindow;

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
	void Init();

public:
	ImVec2 GetSize() const noexcept { return m_Size; } //Just write the values here!

private:
	void RenderFileMenu();
	void RenderWindowMenu();
	void RenderAboutMenu();

private:
	ImVec2 m_Size;

private:
	Core* m_Core						{ nullptr };
	Editor* m_Editor					{ nullptr };
	ImGuiViewport* m_ImGuiViewport		{ nullptr };
	DetailsWindow* m_DetailsWindow		{ nullptr };
	HierarchyWindow* m_HierarchyWindow	{ nullptr };
	ContentBrowser* m_ContentBrowser	{ nullptr };
	DebugLogWindow* m_DebugLogWindow	{ nullptr };
	SystemLogWindow* m_SystemLogWindow	{ nullptr };
};