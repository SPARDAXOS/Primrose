#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class SelectionWindows;
class HierarchyWindow;

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

	inline GameObject* GetTarget() const noexcept { return m_Target; }
	inline void ClearTarget() noexcept { m_Target = nullptr; }
	
	inline ImVec2 GetPosition() const noexcept { return m_Position; }
	inline ImVec2 GetSize() const noexcept { return m_Size; }

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
	GameObject* m_Target	{ nullptr };
	bool m_Opened = true;

	ImVec2 m_Size;
	ImVec2 m_Position;

	const uint32 m_NameInputBufferSize = 33;
	const uint32 m_TagInputBufferSize = 33;
	char m_NameInputBuffer[33];
	char m_TagInputBuffer[33];

private:
	Core* m_Core							{ nullptr };
	Editor* m_Editor						{ nullptr };
	ImGuiViewport* m_ImGuiViewport			{ nullptr };
	SelectionWindows* m_SelectionWindows	{ nullptr };
	HierarchyWindow* m_HierarchyWindow		{ nullptr };
};