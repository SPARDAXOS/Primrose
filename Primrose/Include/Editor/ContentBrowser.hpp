#pragma once
#include "ImGUI/imgui.h"
#include <vector>
#include "Utility.hpp"
#include "Math.hpp"

class Core;
class Editor;
class AssetManager;
class TextureStorage;
class DebugLogWindow;
class SystemLogWindow;
class DetailsWindow;

class Asset;
class Directory;
class Texture2D;

class ContentBrowser final {
public:
	ContentBrowser() = delete;
	ContentBrowser(Core& core, Editor& editor) noexcept;
	~ContentBrowser() = default;

	ContentBrowser(const ContentBrowser&) = delete;
	ContentBrowser& operator=(const ContentBrowser&) = delete;

	ContentBrowser(ContentBrowser&&) = delete;
	ContentBrowser& operator=(ContentBrowser&&) = delete;

public:
	void Update();
	void Render();
	void Init();

public:
	inline void SetSelectedDirectory(Directory& directory) noexcept { m_SelectedDirectory = &directory; }

	inline bool GetState() const noexcept { return m_Opened; }
	inline bool& GetStateRef() noexcept { return m_Opened; }

	inline bool IsContentBrowserWindowHovered() const noexcept { return m_ContentBrowserWindowHovered; }
	inline void ResetWindow() noexcept { m_ResetWindow = true; }

public:
	inline ImVec2 GetTotalCurrentSize() const noexcept { 
		ImVec2 Total;
		Total.x = m_ContentBrowserWindowCurrentSize.x + m_DirectoryExplorerWindowCurrentSize.x;
		Total.y = m_ContentBrowserWindowCurrentSize.y + m_DirectoryExplorerWindowCurrentSize.y;
		return Total;
	}
	inline ImVec2 GetTotalDockSize() const noexcept { 
		ImVec2 Total;
		Total.x = m_ContentBrowserWindowDockSize.x + m_DirectoryExplorerWindowDockSize.x;
		Total.y = m_ContentBrowserWindowDockSize.y + m_DirectoryExplorerWindowDockSize.y;
		return Total;
	}

	inline ImVec2 GetContentBrowserWindowCurrentSize() const noexcept { return m_ContentBrowserWindowCurrentSize; }
	inline ImVec2 GetDirectoryExplorerWindowCurrentSize() const noexcept { return m_DirectoryExplorerWindowCurrentSize; }
	inline ImVec2 GetContentBrowserWindowDockSize() const noexcept { return m_ContentBrowserWindowDockSize; }
	inline ImVec2 GetDirectoryExplorerWindowDockSize() const noexcept { return m_DirectoryExplorerWindowDockSize; }

	inline ImVec2 GetContentBrowserWindowCurrentPosition() const noexcept { return m_ContentBrowserWindowCurrentPosition; }
	inline ImVec2 GetDirectoryExplorerWindowCurrentPosition() const noexcept { return m_DirectoryExplorerWindowCurrentPosition; }
	inline ImVec2 GetContentBrowserWindowDockPosition() const noexcept { return m_ContentBrowserWindowDockPosition; }
	inline ImVec2 GetDirectoryExplorerWindowDockPosition() const noexcept { return m_DirectoryExplorerWindowDockPosition; }

private:
	void RenderContentBrowser();
	void RenderDirectoryExplorer();

	void RenderContent();
	void RenderFolderEditMenu();
	void RenderAssetEditMenu();

	void UpdateContentBrowserFolderEntries();
	void UpdateContentBrowserAssetEntries();
	void RenderContentBrowserEditMenu();


private:
	void NewContentBrowserFrame() noexcept;
	void FlushContentEntriesTexts();


	void CreateFolderEntry(Directory& folder, void* textureID);
	void CreateAssetEntry(Asset& asset, void* textureID);
	void AddContentElementDataEntry(const std::string& text, bool unsavedChanges);

	void UpdateContentElementCursor() noexcept;

	void CheckFolderEditMenu(Directory& folder);
	void CheckAssetEditMenu(Asset& asset);

private:
	Texture2D* GetAssetTexture(const Asset& asset) noexcept;

	void SetupStyle();
	void ClearStyle();

private:
	void AddFileExplorerEntry(Directory* entry);

private:
	void CheckViewportChanges() noexcept;
	void CheckWindowsChanges() noexcept;
	void UpdateWindowDockData() noexcept;
	void UpdateWindowCurrentData() noexcept;

private:
	ImVec2 m_ContentBrowserWindowDockPosition;
	ImVec2 m_DirectoryExplorerWindowDockPosition;

	ImVec2 m_ContentBrowserWindowDockSize;
	ImVec2 m_DirectoryExplorerWindowDockSize;

	ImVec2 m_ContentBrowserWindowCurrentPosition;
	ImVec2 m_DirectoryExplorerWindowCurrentPosition;

	ImVec2 m_ContentBrowserWindowCurrentSize;
	ImVec2 m_DirectoryExplorerWindowCurrentSize;



	ImVec2 m_LastViewportSize;
	ImVec2 m_LastDetailsWindowSize;

private:
	ImVec2 m_UnsavedChangesIconSize{ 15.0f, 15.0f };
	ImVec2 m_ContentBrowserElementSize{ 100.0f, 100.0f };
	float m_ContentBrowserElementPadding = 50.0f;

	Color m_UnsavedChangesIconColor = Colors::White;



	bool m_ResetWindow = false;
	bool m_LinkedWindowsResized = false;
	bool m_Opened = true;

	bool m_ContentBrowserWindowHovered = false;
	bool m_OpenContentBrowserEditMenu = false;



	Asset* m_AssetEditMenuTarget{ nullptr };
	Directory* m_FolderEditMenuTarget{ nullptr };



	float m_ContentElementCursor = 0.0f;
	uint32 m_ContentLineElementsCount = 0;
	std::vector<std::string> m_QueuedContentTexts; //Could use const char* instead
	std::vector<bool> m_UnsavedChangesEntries;

	void* m_SelectedContentElement{ nullptr };
	bool m_FolderEntryOpened{ false };


	Directory* m_SelectedDirectory{ nullptr };

private:
	Texture2D* m_FolderTexture			{ nullptr };
	Texture2D* m_MissingTexture			{ nullptr };
	Texture2D* m_UnknownAssetTexture	{ nullptr };
	Texture2D* m_MaterialAssetTexture	{ nullptr };
	Texture2D* m_UnsavedChangesTexture	{ nullptr };

private:
	bool m_DirectoryExplorerEditorFilter = false;



private:
	Core* m_Core						{ nullptr };
	Editor* m_Editor					{ nullptr };
	AssetManager* m_AssetManager		{ nullptr };
	TextureStorage* m_TextureStorage	{ nullptr };
	ImGuiViewport* m_ImGuiViewport		{ nullptr };
	DebugLogWindow* m_DebugLogWindow	{ nullptr };
	SystemLogWindow* m_SystemLogWindow	{ nullptr };
	DetailsWindow* m_DetailsWindow		{ nullptr };
};