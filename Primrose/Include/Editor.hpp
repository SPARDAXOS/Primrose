#pragma once
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Logger.hpp"
#include "Utility.hpp"

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
class Asset;
class Camera;

enum class AddressingMode;
enum class FilteringModeMin;
enum class FilteringModeMag;
enum class BlendEquation;
enum class SourceBlendMode;
enum class DestinationBlendMode;


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
	void LogWarning(std::string message) noexcept;
	void LogError(std::string message) noexcept;
	void LogSystem(std::string message) noexcept;

private:
	void Render();
	void StartFrame() const;
	void RenderFrame() const;

private:
	void RenderDetailsMenu();
	void RenderHeirarchyMenu();
	void RenderAddGameObjectButton();
	void RenderDirectoryExplorer();
	void RenderContentBrowser();
	void RenderMainMenuBar();

private:
	void CheckInput();
	void UpdateViewportControls();

private:
	void AddHeirarchyEntry(GameObject* entry);
	void AddFileExplorerEntry(Directory* entry);

private:
	void NewContentBrowserFrame() noexcept;
	void UpdateContentBrowserFolderEntries();
	void UpdateContentBrowserAssetEntries();
	void FlushContentTexts();

private:
	bool AddBlendModeSelectable(SourceBlendMode mode);
	bool AddBlendModeSelectable(DestinationBlendMode mode);
	bool AddAddressingModeSelectable(AddressingMode mode);
	bool AddFilteringModeMinSelectable(FilteringModeMin mode);
	bool AddFilteringModeMagSelectable(FilteringModeMag mode);

private:
	void AddSpacings(uint32 count);
	void AddSeparators(uint32 count);
	void PopStyleVars(uint32 count);
	void SetSelectedGameObject(GameObject* object) noexcept;
	void SetSelectedDirectory(Directory* directory) noexcept;
	void CheckForHoveredWindows();
	Texture2D* GetIconTexture(const Asset& asset);

private:
	void SetupEditorStyle();
	void ClearEditorStyle();
	void SetupContentBrowserStyle();
	void ClearContentBrowserStyle();

private:
	GameObject* m_SelectedGameObject	{ nullptr };
	Directory* m_SelectedDirectory		{ nullptr };
	bool m_IsAnyWindowHovered			{ false };
	char m_NameInputBuffer[33];
	char m_TagInputBuffer[33];

private: //Most of these are relative to each other. Calculate them in runtime and maybe once at the start. Think about this.
	ImVec2 m_ContentWindowSize { 1170.0f, 300.0f };
	ImVec2 m_CurrentContentBrowserWindowSize{ m_ContentWindowSize };
	ImVec2 m_ContentBrowserElementSize{ 100.0f, 100.0f };
	float m_ContentBrowserElementPadding = 50.0f;

	ImVec2 m_DirectoryExplorerWindowSize{ 350.0f, 300.0f };
	ImVec2 m_MainMenuBarSize;

	ImVec2 m_TestPosition;

	ImVec2 m_ContentWindowTabsSize{ m_ContentWindowSize.x, 27.0f }; //27 manually adjusted
	ImVec2 m_ContentWindowTabsPosition{ m_ContentWindowPosition.x, 0.0f };

	
	ImVec2 m_ContentWindowPosition;

	bool m_DetailsWindowOpened = true;
	bool m_HeirarchyWindowOpened = true;
	bool m_ContentBrowserOpened = true;
	bool m_DirectoryExplorerWindowOpened = true;

	bool m_DebugLogOpened = true;
	bool m_SystemLogOpened = true;

	bool m_ContentWindowOpened = false;


	bool m_ContentBrowserFocused = true;
	bool m_DebugLogFocused = false;
	bool m_SystemLogFocused = false;

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


	float m_ContentElementCursor = 0.0f;
	uint32 m_ContentLineElementsCount = 0;
	std::vector<std::string> m_QueuedContentTexts;

	void* m_SelectedContentElement	{ nullptr };
	bool m_FolderEntryOpened		{ false };

private:
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