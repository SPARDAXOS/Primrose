#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class SelectionWindows;
class Material;

struct ImVec2;


class MaterialEditor final {
private:
	enum class SelectorBoxType {
		DIFFUSE,
		AMBIENT,
		SPECULAR
	};
public:
	MaterialEditor() = delete;
	MaterialEditor(Core& core, Editor& editor, SelectionWindows& selectionWindows) noexcept;
	~MaterialEditor() = default;

	MaterialEditor(const MaterialEditor&) = delete;
	MaterialEditor& operator=(const MaterialEditor&) = delete;

	MaterialEditor(MaterialEditor&&) = delete;
	MaterialEditor& operator=(MaterialEditor&&) = delete;

public:
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

	void CalculateSectionNamesOffsets();

private:
	void SetupDiffuseSection();
	void SetupAmbientSection();
	void SetupSpecularSection();

	void SetupSelectorBox(SelectorBoxType type);

	void ApplyStyle();
	void ClearStyle();

	void NewFrame() noexcept;

private:
	const float m_LineStartOffsetMultiplier = 0.05f;
	const float m_SelectorBoxWidthMultiplier = 0.2f;
	const float m_SelectorBoxHeight = 20.0f;
	const float m_NameToInputBoxPaddingMultiplier = 0.05f;

private:
	ImVec2 m_NoneTextSize;
	ImVec2 m_CurrentDiffuseSelectorBoxSize;
	ImVec2 m_CurrentAmbientSelectorBoxSize;
	ImVec2 m_CurrentSpecularSelectorBoxSize;

	float m_LineStartOffset = 0.0f;
	float m_NameToInputBoxPadding = 0.0f;

	float m_DiffuseSectionOffset = 0.0f;
	float m_AmbientSectionOffset = 0.0f;
	float m_SpecularSectionOffset = 0.0f;

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