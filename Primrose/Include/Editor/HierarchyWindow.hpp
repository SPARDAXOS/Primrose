#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class EntityComponentSystem;
class ContentBrowser;
class DetailsWindow;
class MainMenuBar;

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

public:
	void Update();
	void Render();
	void Init();

public:
	inline void SetSelectedGameObject(GameObject& gameObject) noexcept { m_SelectedGameObject = &gameObject; }
	inline GameObject* GetSelectedGameObject() const noexcept { return m_SelectedGameObject; }
	inline void ClearSelectedGameObject() noexcept { m_SelectedGameObject = nullptr; }

	inline bool GetState() const noexcept { return m_Open; }
	inline bool& GetStateRef() noexcept { return m_Open; }
	inline void ResetWindow() noexcept { m_ResetWindow = true; }

private:
	void RenderHierarchy();
	void RenderAddButtonMenu();

	void AddHeirarchyEntry(GameObject* entry);

private:
	void CheckViewportChanges() noexcept;
	void CheckWindowsChanges() noexcept;
	void UpdateDockData() noexcept;

private:
	ImVec2 m_HierarchyWindowDockSize;
	ImVec2 m_HierarchyWindowDockPosition;
	ImVec2 m_HierarchyWindowCurrentSize;
	ImVec2 m_HierarchyWindowCurrentPosition;

	ImVec2 m_AddButtonDockSize;
	ImVec2 m_AddButtonDockPosition;
	ImVec2 m_AddButtonCurrentSize;
	ImVec2 m_AddButtonCurrentPosition;

	ImVec2 m_LastViewportSize;
	ImVec2 m_LastContentBrowserWindowSize;

	GameObject* m_SelectedGameObject	{ nullptr };

private:
	bool m_Open = true;
	bool m_ResetWindow = false;

private:
	Core* m_Core						{ nullptr };
	Editor* m_Editor					{ nullptr };
	EntityComponentSystem* m_ECS		{ nullptr };
	ImGuiViewport* m_ImGuiViewport		{ nullptr };
	MainMenuBar* m_MainMenuBar			{ nullptr };
	ContentBrowser* m_ContentBrowser	{ nullptr };
	DetailsWindow* m_DetailsWindow		{ nullptr };
};