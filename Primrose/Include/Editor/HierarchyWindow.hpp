#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class EntityComponentSystem;
class ContentBrowser;
class DetailsWindow;

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
	void Update();
	void Render();
	void Init();

public:
	inline void SetSelectedGameObject(GameObject& gameObject) noexcept { m_SelectedGameObject = &gameObject; }

	inline bool GetState() const noexcept { return m_Open; }
	inline bool& GetStateRef() noexcept { return m_Open; }

	inline GameObject* GetSelectedGameObject() const noexcept { return m_SelectedGameObject; }
	//ResetWindow()? maybe not since you cant move this window

	inline void ClearSelectedGameObject() noexcept { m_SelectedGameObject = nullptr; }

private:
	void RenderHierarchy();
	void RenderAddGameObjectMenu();

	void AddHeirarchyEntry(GameObject* entry);

private:
	ImVec2 m_Size;
	ImVec2 m_Position;
	bool m_Open = true;
	GameObject* m_SelectedGameObject	{ nullptr };

private:
	Core* m_Core						{ nullptr };
	Editor* m_Editor					{ nullptr };
	EntityComponentSystem* m_ECS		{ nullptr };
	ContentBrowser* m_ContentBrowser	{ nullptr };
	DetailsWindow* m_DetailsWindow		{ nullptr };
	ImGuiViewport* m_ImGuiViewport		{ nullptr };
};