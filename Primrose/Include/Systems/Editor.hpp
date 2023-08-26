#pragma once
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Editor/SelectionWindows.hpp"
#include "Editor/MaterialEditor.hpp"
#include "Editor/DetailsWindow.hpp"
#include "Editor/MainMenuBar.hpp"
#include "Editor/ContentBrowser.hpp"
#include "Editor/DebuggerWindow.hpp"
#include "Editor/SystemDebuggerWindow.hpp"
#include "Editor/HierarchyWindow.hpp"

#include "Utility.hpp"
#include "Math.hpp" //For color only


//TODO: Most of these are not needed here anymore after cleanup!
class Core;
class Window;
class EntityComponentSystem;
class TextureStorage;
class AssetManager;
class Input;
class Time;
class Logger;
class Serializer;
class GameObject;
class Directory;
class Texture2D;
class Material;
class SpriteRenderer;
class Asset;
class Camera;

enum class AddressingMode;
enum class FilteringModeMin;
enum class FilteringModeMag;
enum class BlendEquation;
enum class SourceBlendMode;
enum class DestinationBlendMode;


//TODO: Rework this and make and editor for the user to use.
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

		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_DockingPreview, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));


		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 2.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
	}
	void Clear() override {
		ImGui::PopStyleColor(23);
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

public: //Reconsider these functions and whether they should be marked like this.
	//INTERNAL USE ONLY
	Texture2D* GetIconTexture(const Asset& asset) noexcept;
	//INTERNAL USE ONLY
	void CheckForHoveredWindows();
	//INTERNAL USE ONLY
	void AddSpacings(uint32 count);
	//INTERNAL USE ONLY
	void AddSeparators(uint32 count);
	//INTERNAL USE ONLY
	inline ImVec2 GetUniqueScreenCenterPoint(ImVec2 windowSize) const noexcept {
		return ImVec2(m_ImGuiViewport->Size.x / 2 - windowSize.x / 2, m_ImGuiViewport->Size.y / 2 - windowSize.y / 2);
	}
	//INTERNAL USE ONLY - By ref for EditorStyle..Editor
	inline EditorStyle& GetEditorStyle() noexcept { return m_EditorStyle; }

public:
	[[nodiscard]] inline ImGuiViewport* GetGUIViewport() const noexcept { return m_ImGuiViewport; }
	[[nodiscard]] inline SelectionWindows* GetSelectionWindows() const noexcept { return m_SelectionWindows.get(); }
	[[nodiscard]] inline MaterialEditor* GetMaterialEditor() const noexcept { return m_MaterialEditor.get(); }
	[[nodiscard]] inline DetailsWindow* GetDetailsWindow() const noexcept { return m_DetailsWindow.get(); }
	[[nodiscard]] inline MainMenuBar* GetMainMenuBar() const noexcept { return m_MainMenuBar.get(); }
	[[nodiscard]] inline ContentBrowser* GetContentBrowser() const noexcept { return m_ContentBrowser.get(); }
	[[nodiscard]] inline DebuggerWindow* GetDebuggerWindow() const noexcept { return m_DebuggerWindow.get(); }
	[[nodiscard]] inline SystemDebuggerWindow* GetSystemDebuggerWindow() const noexcept { return m_SystemDebuggerWindow.get(); }
	[[nodiscard]] inline HierarchyWindow* GetHierarchyWindowWindow() const noexcept { return m_HierarchyWindow.get(); }
	

private:
	void Render();
	void StartFrame() const;
	void RenderFrame() const;

private:
	void RenderViewportWindow();


private:
	void UpdateEditorInput();
	void UpdateViewportCameraInput();

	void InitializeSubSystems();


private:

	//Inline?
	void SetSelectedGameObject(GameObject* object) noexcept;
	void SetSelectedDirectory(Directory* directory) noexcept;

	//inline void SetMaterialEditorTarget(Material* target) noexcept { m_MaterialEditorTarget = target; }

	void OpenAsset(Asset& asset);

private:
	void UpdateWindowPosition();

	bool IsPointInBoundingBox(ImVec2 point, ImVec2 position, ImVec2 size) const noexcept;

private:

	bool m_IsAnyWindowHovered			{ false };

private: 
	ImVec2 m_Size; //Bar?

	ImVec2 m_NewStandaloneWindowSize;
	
	ImVec2 m_ViewportSize;

private:

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


	bool m_SubSystemsInitialized = false;


private:
	EditorRedStyle m_EditorStyle;

private:
	std::unique_ptr<SelectionWindows> m_SelectionWindows				{ nullptr };
	std::unique_ptr<MaterialEditor> m_MaterialEditor					{ nullptr };
	std::unique_ptr<DetailsWindow> m_DetailsWindow						{ nullptr };
	std::unique_ptr<MainMenuBar> m_MainMenuBar							{ nullptr };
	std::unique_ptr<ContentBrowser> m_ContentBrowser					{ nullptr };
	std::unique_ptr<DebuggerWindow> m_DebuggerWindow					{ nullptr };
	std::unique_ptr<SystemDebuggerWindow> m_SystemDebuggerWindow		{ nullptr };
	std::unique_ptr<HierarchyWindow> m_HierarchyWindow					{ nullptr };

private: //Most of these are not needed here anymore!
	Core* m_Core						{ nullptr };
	Window* m_Window					{ nullptr };
	EntityComponentSystem* m_ECS		{ nullptr };
	TextureStorage* m_TextureStorage	{ nullptr };
	AssetManager* m_AssetManager		{ nullptr };
	Input* m_Input						{ nullptr };
	Time* m_Time						{ nullptr };
	Serializer* m_Serializer			{ nullptr };
	Logger* m_Logger					{ nullptr };

private:
	ImGuiContext* m_ImGuiContext		{ nullptr };
	ImGuiViewport* m_ImGuiViewport		{ nullptr };
	ImGuiIO* m_IO						{ nullptr };
};