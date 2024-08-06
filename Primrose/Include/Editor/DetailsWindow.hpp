#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class SelectionWindow;
class HierarchyWindow;
class ContentBrowser;
class MainMenuBar;

class GameObject;

enum class SourceBlendMode;
enum class DestinationBlendMode;
enum class AddressingMode;
enum class FilteringModeMin;
enum class FilteringModeMag;

struct ImVec2;

class DetailsWindow final {
public:
	DetailsWindow() = delete;
	DetailsWindow(Core& core, Editor& editor) noexcept;
	~DetailsWindow() = default;

	DetailsWindow(const DetailsWindow&) = delete;
	DetailsWindow& operator=(const DetailsWindow&) = delete;

	DetailsWindow(DetailsWindow&&) = delete;
	DetailsWindow& operator=(DetailsWindow&&) = delete;

public:
	void Update();
	void Render();
	void Init();

public:
	inline void SetState(bool state) noexcept { m_Opened = state; }
	inline bool GetState() const noexcept { return m_Opened; }
	inline bool& GetStateRef() noexcept { return m_Opened; }

	inline void ResetWindow() noexcept { m_ResetWindow = true; }

	inline GameObject* GetTarget() const noexcept { return m_Target; }
	inline void ClearTarget() noexcept { m_Target = nullptr; }
	
	inline ImVec2 GetDockPosition() const noexcept { return m_WindowDockPosition; }
	inline ImVec2 GetDockSize() const noexcept { return m_WindowDockSize; }

	inline ImVec2 GetCurrentPosition() const noexcept { return m_WindowCurrentPosition; }
	inline ImVec2 GetCurrentSize() const noexcept { return m_WindowCurrentSize; }

	inline char* GetNameInputBuffer() noexcept { return m_NameInputBuffer; }
	inline uint32 GetNameInputBufferSize() const noexcept { return m_NameInputBufferSize; }

	inline char* GetTagInputBuffer() noexcept { return m_TagInputBuffer; }
	inline uint32 GetTagInputBufferSize() const noexcept { return m_TagInputBufferSize; }


private:
	void RenderInfoDetails();
	void RenderTransformDetails();
	void RenderSpriteRendererDetails();
	void RenderSkeletalMeshDetails();
	void RenderDirectionalLightDetails();
	void RenderPointLightDetails();
	void RenderSpotLightDetails();
	void RenderAddComponentMenu();

private:
	bool AddBlendModeSelectable(SourceBlendMode mode);
	bool AddBlendModeSelectable(DestinationBlendMode mode);
	bool AddAddressingModeSelectable(AddressingMode mode);
	bool AddFilteringModeMinSelectable(FilteringModeMin mode);
	bool AddFilteringModeMagSelectable(FilteringModeMag mode);

	void UpdateTarget() noexcept;

private:
	void CheckViewportChanges() noexcept;
	void CheckWindowsChanges() noexcept;
	void UpdateWindowDockData() noexcept;
	void UpdateWindowCurrentData() noexcept;

private:
	GameObject* m_Target	{ nullptr };
	bool m_Opened = true;
	bool m_ResetWindow = false;
	bool m_LinkedWindowsResized = false;

	ImVec2 m_WindowDockSize;
	ImVec2 m_WindowDockPosition;

	ImVec2 m_WindowCurrentSize;
	ImVec2 m_WindowCurrentPosition;

	ImVec2 m_LastViewportSize;
	ImVec2 m_LastContentBrowserWindowSize;



	const uint32 m_NameInputBufferSize = 33;
	const uint32 m_TagInputBufferSize = 33;
	char m_NameInputBuffer[33];
	char m_TagInputBuffer[33];

private:
	Core* m_Core							{ nullptr };
	Editor* m_Editor						{ nullptr };
	ImGuiViewport* m_ImGuiViewport			{ nullptr };
	SelectionWindow* m_SelectionWindows	{ nullptr };
	ContentBrowser* m_ContentBrowser		{ nullptr };
	HierarchyWindow* m_HierarchyWindow		{ nullptr }; //?
	MainMenuBar* m_MainMenuBar				{ nullptr };
};