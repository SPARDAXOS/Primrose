#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class AssetManager;
class TextureStorage;
class DetailsWindow;
class DebugLogWindow;
class SystemLogWindow;

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

	inline bool GetState() const noexcept { return m_ContentBrowserWindowOpened; }
	inline bool& GetStateRef() noexcept { return m_ContentBrowserWindowOpened; }

	inline ImVec2 GetContentBrowserWindowSize() const noexcept { return m_ContentBrowserWindowSize; }
	inline ImVec2 GetDirectoryExplorerWindowSize() const noexcept { return m_DirectoryExplorerWindowSize; }
	inline ImVec2 GetContentBrowserWindowDockSize() const noexcept { return m_ContentBrowserWindowDockSize; }
	inline ImVec2 GetDirectoryExplorerWindowDockSize() const noexcept { return m_DirectoryExplorerWindowDockSize; }

	inline ImVec2 GetContentBrowserWindowPosition() const noexcept { return m_ContentBrowserWindowPosition; }
	inline ImVec2 GetDirectoryExplorerWindowPosition() const noexcept { return m_DirectoryExplorerWindowPosition; }
	inline ImVec2 GetContentBrowserWindowDockPosition() const noexcept { return m_ContentBrowserWindowDockPosition; }
	inline ImVec2 GetDirectoryExplorerWindowDockPosition() const noexcept { return m_DirectoryExplorerWindowDockPosition; }


	inline bool IsContentBrowserWindowHovered() const noexcept { return m_ContentBrowserWindowHovered; }

	inline void ResetWindow() noexcept { m_ContentBrowserWindowReset = true; } //?? hmmm

private:
	void RenderContentBrowser();
	void RenderDirectoryExplorer();

	void NewContentBrowserFrame() noexcept;
	void UpdateContentBrowserFolderEntries();
	void UpdateContentBrowserAssetEntries();
	void UpdateContentBrowserMenu();
	void FlushContentTexts();

	void SetupStyle();
	void ClearStyle();

private:
	void AddFileExplorerEntry(Directory* entry);

private:
	ImVec2 m_ContentBrowserWindowDockPosition;
	ImVec2 m_DirectoryExplorerWindowDockPosition;

	ImVec2 m_ContentBrowserWindowDockSize;
	ImVec2 m_DirectoryExplorerWindowDockSize;


	ImVec2 m_ContentBrowserWindowPosition;
	ImVec2 m_DirectoryExplorerWindowPosition;

	ImVec2 m_ContentBrowserWindowSize; //Hmmmm.
	ImVec2 m_DirectoryExplorerWindowSize;

	bool m_ContentBrowserWindowReset = true;
	bool m_DirectoryExplorerWindowReset = true;



	ImVec2 m_ContentBrowserElementSize{ 100.0f, 100.0f };
	float m_ContentBrowserElementPadding = 50.0f;

	bool m_ContentBrowserWindowOpened = true;
	bool m_DirectoryExplorerWindowOpened = true; //Should use the one above instead for both of them!

	bool m_ContentBrowserWindowHovered = false;
	bool m_OpenContentBrowserEditMenu = false;



	Asset* m_AssetEditMenuTarget{ nullptr };
	Directory* m_FolderEditMenuTarget{ nullptr };



	float m_ContentElementCursor = 0.0f;
	uint32 m_ContentLineElementsCount = 0;
	std::vector<std::string> m_QueuedContentTexts; //Could use const char* instead

	void* m_SelectedContentElement{ nullptr };
	bool m_FolderEntryOpened{ false };


	Directory* m_SelectedDirectory{ nullptr };

private:
	Texture2D* m_FolderTexture{ nullptr };
	Texture2D* m_MaterialAssetTexture{ nullptr };

private:
	bool m_DirectoryExplorerEditorFilter = false;



private:
	Core* m_Core						{ nullptr };
	Editor* m_Editor					{ nullptr };
	AssetManager* m_AssetManager		{ nullptr };
	TextureStorage* m_TextureStorage	{ nullptr };
	ImGuiViewport* m_ImGuiViewport		{ nullptr };
	
	DetailsWindow* m_DetailsWindow		{ nullptr };
	DebugLogWindow* m_DebugLogWindow	{ nullptr };
	SystemLogWindow* m_SystemLogWindow	{ nullptr };
};