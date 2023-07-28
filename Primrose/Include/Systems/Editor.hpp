#pragma once
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Tools/Logger.hpp"
#include "Utility.hpp"
#include "Math.hpp" //For color only

class Core;
class Window;
class EntityComponentSystem;
class TextureStorage;
class AssetManager;
class Input;
class Time;
class GameObject;
class Directory;
class Texture2D;
class SpriteRenderer;
class Asset;
class Camera;

enum class AddressingMode;
enum class FilteringModeMin;
enum class FilteringModeMag;
enum class BlendEquation;
enum class SourceBlendMode;
enum class DestinationBlendMode;



class EditorStyle {
public:
	virtual void Apply() = 0;
	virtual void Clear() = 0;

	Color m_MainColor;
};
class EditorRedStyle final : public EditorStyle {
public:
	EditorRedStyle() noexcept
	{
		m_MainColor = Colors::Red;
	}

	void Apply() override {

		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.05f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_TabUnfocused, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_DockingPreview, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));


		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
	}
	void Clear() override {
		ImGui::PopStyleColor(21);
		ImGui::PopStyleVar(2);
	}


};




class Editor final {
public:
	Editor() = delete;
	Editor(Core& core);
	~Editor();

	Editor(const Editor&) = delete;
	Editor& operator=(const Editor&) = delete;

	Editor(Editor&&) = delete;
	Editor& operator=(Editor&&) = delete;

public:
	[[nodiscard]] bool Update();

public:
	void SaveEngineTexturesReferences();

	void DebugLog(std::string message) noexcept;
	void WarningLog(std::string message) noexcept;
	void ErrorLog(std::string message) noexcept;
	void SystemLog(std::string message) noexcept;

private:
	void Render();
	void StartFrame() const;
	void RenderFrame() const;

private:
	void RenderHeirarchyMenu();
	void RenderAddGameObjectMenu();
	void RenderDirectoryExplorer();
	void RenderMainMenuBar();
	void RenderViewportWindow();

	void RenderDetailsMenu();
	void RenderInfoDetails();
	void RenderTransformDetails();
	void RenderSpriteRendererDetails();
	void RenderAddComponentMenu();

	void RenderContentWindows();
	void RenderContentBrowser();
	void RenderDebugLog();
	void RenderSystemLog();


private:
	void CheckInput();
	void UpdateViewportControls();

private:
	void AddHeirarchyEntry(GameObject* entry);
	void AddFileExplorerEntry(Directory* entry);

private:
	void NewContentBrowserFrame() noexcept;
	void NewSpriteSelectorFrame() noexcept;
	void UpdateContentBrowserFolderEntries();
	void UpdateContentBrowserAssetEntries();
	void UpdateSpriteSelectorEntries();
	void FlushContentTexts();
	void FlushSpriteSelectorTexts();

private:
	bool AddBlendModeSelectable(SourceBlendMode mode);
	bool AddBlendModeSelectable(DestinationBlendMode mode);
	bool AddAddressingModeSelectable(AddressingMode mode);
	bool AddFilteringModeMinSelectable(FilteringModeMin mode);
	bool AddFilteringModeMagSelectable(FilteringModeMag mode);

private:
	void AddSpacings(uint32 count);
	void AddSeparators(uint32 count);
	void SetSelectedGameObject(GameObject* object) noexcept;
	void SetSelectedDirectory(Directory* directory) noexcept;
	inline void SetSpriteRendererEditTarget(SpriteRenderer* target) noexcept { m_SpriteRendererEditTarget = target; }
	void CheckForHoveredWindows();
	Texture2D* GetIconTexture(const Asset& asset);

private:
	void SetupContentBrowserStyle();
	void ClearContentBrowserStyle();
	void UpdateWindowPositions();

	bool IsPointInBoundingBox(ImVec2 point, ImVec2 position, ImVec2 size) const noexcept;
	inline ImVec2 GetContentWindowStartPosition(ImVec2 windowSize) const noexcept {
		return ImVec2(m_GUIViewport->Size.x / 2 - windowSize.x / 2, m_GUIViewport->Size.y / 2 - windowSize.y / 2);
	}

private:
	GameObject* m_SelectedGameObject			{ nullptr };
	Directory* m_SelectedDirectory				{ nullptr };
	SpriteRenderer* m_SpriteRendererEditTarget	{ nullptr };

	bool m_IsAnyWindowHovered					{ false };
	char m_NameInputBuffer[33];
	char m_TagInputBuffer[33];

private: 
	ImVec2 m_ContentBrowserWindowSize;
	ImVec2 m_DetailsWindowSize;
	ImVec2 m_HierarchyWindowSize;
	ImVec2 m_DirectoryExplorerWindowSize;
	ImVec2 m_MainMenuBarSize;
	ImVec2 m_NewContentWindowSize;

	ImVec2 m_SpriteSelectorWindowSize;
	
	ImVec2 m_ContentBrowserWindowPosition;
	ImVec2 m_DetailsWindowPosition;
	ImVec2 m_HierarchyWindowPosition;
		
	ImVec2 m_ContentBrowserElementSize	{ 100.0f, 100.0f };
	float m_ContentBrowserElementPadding = 50.0f;

	ImVec2 m_SpriteSelectorElementSize	{ 100.0f, 100.0f };
	float m_SpriteSelectorElementPadding = 50.0f;
	

	bool m_DetailsWindowOpened = true;
	bool m_HeirarchyWindowOpened = true;
	bool m_DirectoryExplorerWindowOpened = true;

	bool m_ContentBrowserOpened = true;
	bool m_DebugLogOpened = false;
	bool m_SystemLogOpened = false;

	bool m_SpriteSelectorOpened = false;

	bool m_ContentBrowserWindowReset = true;
	bool m_DebugLogWindowReset = true;
	bool m_SystemLogWindowReset = true;

private:
	Camera* m_ViewportCameraReference{ nullptr };

	float m_FreeLookSensitivity = 0.1f;
	float m_FreeLookSpeed = 10.0f;

	float m_CameraMovementSpeed = 5.0f;
	float m_CameraSpeedMax = 10.0f;
	float m_CameraSpeedMin = 1.0f;
	float m_CameraSpeedDecrease = 20.0f;
	float m_CameraSpeedIncrease = 20.0f;

	bool m_ViewportNavigationMode = false;

private:
	Texture2D* m_FolderTexture	 { nullptr };
	Texture2D* m_DebugTexture    { nullptr };
	Texture2D* m_WarningTexture  { nullptr };
	Texture2D* m_ErrorTexture    { nullptr };

private:
	float m_ContentElementCursor = 0.0f;
	uint32 m_ContentLineElementsCount = 0;
	std::vector<std::string> m_QueuedContentTexts; //Could use const char* instead

	void* m_SelectedContentElement	{ nullptr };
	bool m_FolderEntryOpened		{ false };

private:
	float m_SpriteSelectorElementCursor = 0.0f;
	uint32 m_SpriteSelectorLineElementsCount = 0;
	std::vector<std::string> m_QueuedSpriteSelectorTexts; //Could use const char* instead

	void* m_SelectedSpriteSelectorElement	{ nullptr };


private:
	EditorRedStyle m_EditorStyle;
	Logger m_Logger{ *m_TimeReference };

private:
	Core* m_EngineCore							{ nullptr };
	Window* m_WindowReference					{ nullptr };
	EntityComponentSystem* m_ECSReference		{ nullptr };
	TextureStorage* m_TextureStorageReference	{ nullptr };
	AssetManager* m_AssetManagerReference		{ nullptr };
	Input* m_InputReference						{ nullptr };
	Time* m_TimeReference						{ nullptr };

private:
	ImGuiContext* m_GUIContext		{ nullptr };
	ImGuiViewport* m_GUIViewport	{ nullptr };
	ImGuiIO* m_IO					{ nullptr };
};