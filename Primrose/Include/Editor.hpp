#pragma once
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Window.hpp"
#include <iostream>


class Editor final {
public:
	Editor() = delete;
	Editor(Window& window, EntityComponentSystem& ecs)
		: m_WindowReference(&window), m_ECSReference(&ecs)
	{
		IMGUI_CHECKVERSION();
		m_GUIContext = ImGui::CreateContext();
		m_GUIViewport = ImGui::GetMainViewport();

		m_IO = &ImGui::GetIO();
		m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// IF using Docking Branch

		ImGui_ImplGlfw_InitForOpenGL(m_WindowReference->GetWindowResource().m_ptr, true);
		ImGui_ImplOpenGL3_Init();
	}
	~Editor() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	Editor(const Editor&) = delete;
	Editor& operator=(const Editor&) = delete;

	Editor(Editor&&) = delete;
	Editor& operator=(Editor&&) = delete;

public:
	[[nodiscard]] bool Update() {

		Render();

		//Clear selected on right click

		return true;
	}

private:
	void Render() {//TODO: REturn const to this after getting rid of the text transform

		StartFrame();
		
		RenderDetailsMenu();
		RenderHeirarchyMenu();

		RenderFrame();
	}
	void StartFrame() const {
		//Feed input and start GUI frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow();
	}
	void RenderFrame() const {
		//Render GUI to screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}


	void RenderDetailsMenu()  { //TODO: REturn const to this after getting rid of the text transform


		//Notes:
		//This could be abstracted away since DRY


		//TODO: Add the ability to reorder these one day
		//TODO: Move viewport related code to this class


		//Render GUI
		//Note: Will also lock position and size
		ImGui::SetNextWindowSize(ImVec2(400.0f, m_GUIViewport->Size.y));
		ImGui::SetNextWindowPos(ImVec2(m_GUIViewport->Size.x - 400.0f, 0.0f));

		ImGui::Begin("Details");

		if (m_SelectedGameObject != nullptr) {
			////////////
			//Transform
			////////////
			Transform* SelectedTransform = &m_SelectedGameObject->GetTransform();
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGui::Text("Position");

				ImGui::SameLine(100.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text("X");
				ImGui::PopStyleColor();

				ImGui::SameLine(120.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##PX", &SelectedTransform->m_Position.m_X);

				ImGui::SameLine(200.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImGui::Text("Y");
				ImGui::PopStyleColor();

				ImGui::SameLine(220.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##PY", &SelectedTransform->m_Position.m_Y);

				ImGui::SameLine(300.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				ImGui::Text("Z");
				ImGui::PopStyleColor();

				ImGui::SameLine(320.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##PZ", &SelectedTransform->m_Position.m_Z);


				ImGui::Text("Rotation");

				ImGui::SameLine(100.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text("X");
				ImGui::PopStyleColor();

				ImGui::SameLine(120.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##RX", &SelectedTransform->m_Rotation.m_X);

				ImGui::SameLine(200.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImGui::Text("Y");
				ImGui::PopStyleColor();

				ImGui::SameLine(220.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##RY", &SelectedTransform->m_Rotation.m_Y);

				ImGui::SameLine(300.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				ImGui::Text("Z");
				ImGui::PopStyleColor();

				ImGui::SameLine(320.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##RZ", &SelectedTransform->m_Rotation.m_Z);


				ImGui::Text("Scale");

				ImGui::SameLine(100.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text("X");
				ImGui::PopStyleColor();

				ImGui::SameLine(120.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##SX", &SelectedTransform->m_Scale.m_X);

				ImGui::SameLine(200.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImGui::Text("Y");
				ImGui::PopStyleColor();

				ImGui::SameLine(220.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##SY", &SelectedTransform->m_Scale.m_Y);

				ImGui::SameLine(300.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				ImGui::Text("Z");
				ImGui::PopStyleColor();

				ImGui::SameLine(320.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##SZ", &SelectedTransform->m_Scale.m_Z);

			}

			////////////
			//SpriteRenderer
			////////////
			if (m_SelectedGameObject->HasComponent<SpriteRenderer>()) {
				if (ImGui::CollapsingHeader("SpriteRenderer")) {


				}
			}
		}

		ImGui::End();
	}
	void RenderHeirarchyMenu() {

		ImGui::SetNextWindowSize(ImVec2(200.0f, m_GUIViewport->Size.y));
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::Begin("Heirarchy");

		//TODO: This needs cleanup, especially the id check down there.
		std::vector<GameObject*> GameObjects = m_ECSReference->GetGameObjects();
		for (uint32 i = 0; i < GameObjects.size(); i++) {
			if (GameObjects.at(i)->GetObjectID() != VIEWPORT_CAMERA_OBJECT_ID && GameObjects.at(i)->GetParent() == nullptr)
				AddHeirarchyEntry(GameObjects.at(i));
		}

		ImGui::End();
	}


	void AddHeirarchyEntry(GameObject* entry) {

		if (entry == nullptr)
			return;

		bool IsLeaf = false;
		if (!entry->HasChildren())
			IsLeaf = true;

		if (IsLeaf) {

			bool IsOpen = false;
			if (ImGui::TreeNodeEx(entry->GetName().data(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnArrow)) {

				IsOpen = true;

				if (ImGui::IsItemClicked()) {
					m_SelectedGameObject = entry;

				}

				ImGui::TreePop();
			}
		}
		else {
			if (ImGui::TreeNodeEx(entry->GetName().data(), ImGuiTreeNodeFlags_OpenOnArrow)) {

				if (ImGui::IsItemClicked()) {
					m_SelectedGameObject = entry;

				}

				std::vector<GameObject*> Children = entry->GetChildren();
				for (auto& x : Children)
					AddHeirarchyEntry(x);

				ImGui::TreePop();
			}
		}
	}

public:
	GameObject* m_SelectedGameObject;

private:
	Window* m_WindowReference;
	EntityComponentSystem* m_ECSReference;

private:
	ImGuiContext* m_GUIContext;
	ImGuiViewport* m_GUIViewport;
	ImGuiIO* m_IO;
};