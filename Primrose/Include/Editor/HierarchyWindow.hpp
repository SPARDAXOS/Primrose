#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class ContentBrowser;

class GameObject;


class HierarchyWindow final {
public:
	HierarchyWindow() = delete;
	HierarchyWindow(Core& core, Editor& editor) noexcept;
	~HierarchyWindow() = default;

	HierarchyWindow(const HierarchyWindow&) = delete;
	HierarchyWindow& operator=(const HierarchyWindow&) = delete;

	HierarchyWindow(HierarchyWindow&&) = delete;
	HierarchyWindow& operator=(HierarchyWindow&&) = delete;
	//Separate explorer from content? probably not
public:
	void Render();
	void Init();

public:
	bool GetState() const noexcept { return m_Open; }

private:
	void RenderHierarchy();
	void RenderAddGameObjectMenu();

	void AddHeirarchyEntry(GameObject* entry);

private:
	ImVec2 m_Size;
	ImVec2 m_Position;
	bool m_Open = true;

private:
	Core* m_Core						{ nullptr };
	Editor* m_Editor					{ nullptr };
	ContentBrowser* m_ContentBrowser	{ nullptr };
	ImGuiViewport* m_ImGuiViewport		{ nullptr };
};