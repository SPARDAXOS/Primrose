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

	inline void SetChangesCheckData(bool& target, std::string_view name) noexcept { m_ChangesCheckTarget = &target; m_ChangesCheckName = name; }

	//Notes
	//They share code so combine them
	//The only difference is the target and the type of entries
	//You CANT open multiple selectors at the same time i think!

	//Bugs: 
	//-1 SpriteSelector window stops opening
	//-2 MaterialSelector window doesnt close if i click out of it! Could have effect on bug 1

	//IMPORTANT NOTE: Rename the sprite selector to texture selector since that makes more sense!

private: //TextureSelector
	void RenderSpriteSelector();
	void UpdateSpriteSelectorEntries();
	void NewSpriteSelectorFrame() noexcept;
	void UpdateSpriteSelectorCursor() noexcept;
	void CreateTextureSelectorEntry(Texture2D& texture);
	void AddSpriteEntryData(const std::string_view& text);
	void FlushSpriteSelectorTexts();

private: //MaterialSelector
	void RenderMaterialSelector();
	void UpdateMaterialSelectorEntries();
	void NewMaterialSelectorFrame() noexcept;
	void UpdateMaterialSelectorCursor() noexcept;
	void CreateMaterialSelectorEntry(Material& material, void* textureID);
	void AddMaterialEntryData(const std::string_view& text);
	void FlushMaterialSelectorTexts();

private:
	void CheckViewportChanges();
	void CheckChanges(const std::string& newName) noexcept; //dfopes this include materials?

	void SetupStyle();
	void ClearStyle();

private: //Sprite Selector
	bool m_SpriteSelectorOpened = false;
	ImVec2 m_SpriteSelectorWindowSize;
	ImVec2 m_SpriteSelectorElementSize{ 100.0f, 100.0f };
	float m_SpriteSelectorElementPadding = 50.0f;
	Texture2D** m_SpriteSelectorTarget{ nullptr };
	//------------
	float m_SpriteSelectorElementCursor = 0.0f;
	uint32 m_SpriteSelectorLineElementsCount = 0;
	std::vector<std::string_view> m_QueuedSpriteSelectorTexts; //Could use const char* instead
	void* m_SelectedSpriteSelectorElement{ nullptr }; //Why is this even void*?

private: //Material Selector
	bool m_MaterialSelectorOpened = false;
	ImVec2 m_MaterialSelectorWindowSize;
	ImVec2 m_MaterialSelectorElementSize{ 100.0f, 100.0f };
	float m_MaterialSelectorElementPadding = 50.0f;
	Material** m_MaterialSelectorTarget{ nullptr };
	//------------
	float m_MaterialSelectorElementCursor = 0.0f;
	uint32 m_MaterialSelectorLineElementsCount = 0;
	std::vector<std::string_view> m_QueuedMaterialSelectorTexts; //Could use const char* instead
	void* m_SelectedMaterialSelectorElement{ nullptr };

private:
	bool* m_ChangesCheckTarget	{ nullptr };
	std::string m_ChangesCheckName;

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