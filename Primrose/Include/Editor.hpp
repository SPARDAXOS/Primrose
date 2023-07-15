#pragma once
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Window.hpp"


class Editor final {
public:
	Editor() = delete;
	Editor(Window& window)
		: m_WindowReference(&window)
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
	void RenderDetailsMenu()  { //TODO: REturn const to this after getting rid of the text transform


		//Notes:
		//This could be abstracted away since DRY
		//I get the selected GameObject from ECS? or Somewhere at least and do all this if its valid


		//TODO: Add the ability to reorder these one day
		//TODO: Move viewport related code to this class


		//Render GUI
		//Note: Will also lock position and size
		ImGui::SetNextWindowSize(ImVec2(400.0f, m_GUIViewport->Size.y));
		ImGui::SetNextWindowPos(ImVec2(m_GUIViewport->Size.x - 400.0f, 0.0f));

		ImGui::Begin("Details");
		if (ImGui::CollapsingHeader("Transform")) {

			ImGui::Text("Position");

			ImGui::SameLine(100.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::Text("X");
			ImGui::PopStyleColor();

			ImGui::SameLine(120.0f);
			ImGui::PushItemWidth(50.0F);
			ImGui::InputFloat("##PX", &m_TransformTest->m_Position.m_X);

			ImGui::SameLine(200.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			ImGui::Text("Y");
			ImGui::PopStyleColor();

			ImGui::SameLine(220.0f);
			ImGui::PushItemWidth(50.0F);
			ImGui::InputFloat("##PY", &m_TransformTest->m_Position.m_Y);

			ImGui::SameLine(300.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
			ImGui::Text("Z");
			ImGui::PopStyleColor();

			ImGui::SameLine(320.0f);
			ImGui::PushItemWidth(50.0F);
			ImGui::InputFloat("##PZ", &m_TransformTest->m_Position.m_Z);


			ImGui::Text("Rotation");

			ImGui::SameLine(100.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::Text("X");
			ImGui::PopStyleColor();

			ImGui::SameLine(120.0f);
			ImGui::PushItemWidth(50.0F);
			ImGui::InputFloat("##RX", &m_TransformTest->m_Rotation.m_X);

			ImGui::SameLine(200.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			ImGui::Text("Y");
			ImGui::PopStyleColor();

			ImGui::SameLine(220.0f);
			ImGui::PushItemWidth(50.0F);
			ImGui::InputFloat("##RY", &m_TransformTest->m_Rotation.m_Y);

			ImGui::SameLine(300.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
			ImGui::Text("Z");
			ImGui::PopStyleColor();

			ImGui::SameLine(320.0f);
			ImGui::PushItemWidth(50.0F);
			ImGui::InputFloat("##RZ", &m_TransformTest->m_Rotation.m_Z);


			ImGui::Text("Scale");

			ImGui::SameLine(100.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
			ImGui::Text("X");
			ImGui::PopStyleColor();

			ImGui::SameLine(120.0f);
			ImGui::PushItemWidth(50.0F);
			ImGui::InputFloat("##SX", &m_TransformTest->m_Scale.m_X);

			ImGui::SameLine(200.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
			ImGui::Text("Y");
			ImGui::PopStyleColor();

			ImGui::SameLine(220.0f);
			ImGui::PushItemWidth(50.0F);
			ImGui::InputFloat("##SY", &m_TransformTest->m_Scale.m_Y);

			ImGui::SameLine(300.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
			ImGui::Text("Z");
			ImGui::PopStyleColor();

			ImGui::SameLine(320.0f);
			ImGui::PushItemWidth(50.0F);
			ImGui::InputFloat("##SZ", &m_TransformTest->m_Scale.m_Z);

		}
		if (ImGui::CollapsingHeader("SpriteRenderer")) {


		}

		ImGui::End();
	}
	void RenderHeirarchyMenu() const {

		//TODO: Temporary make the heirarchy and use the selected GameObject from it in the details tab work
		//TODO: To do the parent thing, do only the ones that their parent is NOT the scene and their parent. You will end up with the heirarchy.

		//TODO: For this to work, i need to change how the parent code works in GO. Make it store references to its children. SetParent(), AddChild(GameObject& *this)

		//TODO: Add more Children support for GameObject class. FindChild() AddChild() GetChildren()
		ImGui::SetNextWindowSize(ImVec2(200.0f, m_GUIViewport->Size.y));
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::Begin("Heirarchy");

		//Get gameobjects from Ecs
		//Create entry for each one using the data from the game objects







		ImGui::End();
	}


	void RenderFrame() const {
		//Render GUI to screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

public:
	Transform* m_TransformTest;


private:
	Window* m_WindowReference;
	ImGuiContext* m_GUIContext;
	ImGuiViewport* m_GUIViewport;
	ImGuiIO* m_IO;
};