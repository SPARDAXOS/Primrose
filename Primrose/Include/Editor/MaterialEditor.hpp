#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class SelectionWindows;
class Material;

struct ImVec2;


class MaterialEditor final {
public:
	MaterialEditor() = delete;
	MaterialEditor(Core& core, Editor& editor, SelectionWindows& selectionWindows) noexcept;
	~MaterialEditor() = default;

	MaterialEditor(const MaterialEditor&) = delete;
	MaterialEditor& operator=(const MaterialEditor&) = delete;

	MaterialEditor(MaterialEditor&&) = delete;
	MaterialEditor& operator=(MaterialEditor&&) = delete;

public:
	void Update();
	void Render();

public:
	inline void SetTarget(Material* target) noexcept { m_Target = target; }
	inline void SetWindowState(bool state) noexcept { 

		if (!m_Opened && state)
			m_WindowSizeReset = true;
			m_Opened = state; 
	}

	inline Material* GetTarget() const noexcept { return m_Target; }
	inline bool GetWindowState() const noexcept { return m_Opened; }

private:
	void UpdateWindowPosition();

private:
	Material* m_Target{ nullptr };
	bool m_Opened = false;
	bool m_WindowSizeReset = true;
	ImVec2 m_CurrentWindowSize;

private:
	Core* m_Core							{ nullptr };
	Editor* m_Editor						{ nullptr };
	SelectionWindows* m_SelectionWindows	{ nullptr };
};