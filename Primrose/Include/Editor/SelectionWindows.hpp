#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class TextureStorage;
class AssetManager;
class Texture2D;
class Material;

struct ImVec2;

class SelectionWindows final {
public:
	SelectionWindows() = delete;
	SelectionWindows(Core& core, Editor& editor) noexcept;
	~SelectionWindows() = default;

	SelectionWindows(const SelectionWindows&) = delete;
	SelectionWindows& operator=(const SelectionWindows&) = delete;

	SelectionWindows(SelectionWindows&&) = delete;
	SelectionWindows& operator=(SelectionWindows&&) = delete;

public:
	void Update();
	void Render();
	void Init();

public:
	inline void SetSpriteSelectorTarget(Texture2D*& target) noexcept { m_SpriteSelectorTarget = &target; }
	inline void SetMaterialSelectorTarget(Material*& target) noexcept { m_MaterialSelectorTarget = &target; }
	inline void SetSpriteSelectorWindowState(bool state) noexcept { m_SpriteSelectorOpened = state; }
	inline void SetMaterialSelectorWindowState(bool state) noexcept { m_MaterialSelectorOpened = state; }

	inline Texture2D** GetSpriteSelectorTarget() const noexcept { return m_SpriteSelectorTarget; }
	inline Material**  GetMaterialSelectorTarget() const noexcept { return m_MaterialSelectorTarget; }

	inline void ClearSpriteSelectorTarget() noexcept { m_SpriteSelectorTarget = nullptr; }
	inline void ClearMaterialSelectorTarget() noexcept { m_MaterialSelectorTarget = nullptr; }

private:
	void UpdateSpriteSelectorEntries();
	void UpdateMaterialSelectorEntries();

	void RenderSpriteSelector();
	void RenderMaterialSelector();

	void FlushSpriteSelectorTexts();
	void FlushMaterialSelectorTexts();

	void CheckViewportChanges();

	void SetupStyle();
	void ClearStyle();

private:
	//Sprite Selector
	bool m_SpriteSelectorOpened = false;
	ImVec2 m_SpriteSelectorWindowSize;
	ImVec2 m_SpriteSelectorElementSize{ 100.0f, 100.0f };
	float m_SpriteSelectorElementPadding = 50.0f;
	Texture2D** m_SpriteSelectorTarget{ nullptr };
	float m_SpriteSelectorElementCursor = 0.0f;
	uint32 m_SpriteSelectorLineElementsCount = 0;
	std::vector<std::string> m_QueuedSpriteSelectorTexts; //Could use const char* instead
	void* m_SelectedSpriteSelectorElement{ nullptr }; //Why is this even void*?

	//Material Selector
	bool m_MaterialSelectorOpened = false;
	ImVec2 m_MaterialSelectorWindowSize;
	ImVec2 m_MaterialSelectorElementSize{ 100.0f, 100.0f };
	float m_MaterialSelectorElementPadding = 50.0f;
	Material** m_MaterialSelectorTarget{ nullptr };
	float m_MaterialSelectorElementCursor = 0.0f;
	uint32 m_MaterialSelectorLineElementsCount = 0;
	std::vector<std::string> m_QueuedMaterialSelectorTexts; //Could use const char* instead
	void* m_SelectedMaterialSelectorElement{ nullptr };

private:
	Texture2D* m_MaterialAssetTexture	{ nullptr };
	Texture2D* m_MissingTexture			{ nullptr };

private:
	Core* m_Core						{ nullptr };
	Editor* m_Editor					{ nullptr };
	ImGuiViewport* m_ImGuiViewport		{ nullptr };
	TextureStorage* m_TextureStorage	{ nullptr };
	AssetManager* m_AssetManager		{ nullptr };
};