#include "Editor/MainMenuBar.hpp"
#include "Systems/Core.hpp"
#include "Systems/Editor.hpp"

#include "Editor/HierarchyWindow.hpp"
#include "Editor/ContentBrowser.hpp"
#include "Editor/DebugLogWindow.hpp"
#include "Editor/SystemLogWindow.hpp"


MainMenuBar::MainMenuBar(Core& core, Editor& editor) noexcept
	:	m_Core(&core), m_Editor(&editor)
{
}


void MainMenuBar::Render() {

	if (ImGui::BeginMainMenuBar()) {
		m_Size = ImGui::GetWindowSize();

		RenderFileMenu();
		RenderWindowMenu();
		RenderAboutMenu();
	}

	ImGui::EndMainMenuBar();
}
void MainMenuBar::Init() {

	m_ImGuiViewport = &m_Editor->GetGUIViewport();
	m_DetailsWindow = &m_Editor->GetDetailsWindow();
	m_HierarchyWindow = &m_Editor->GetHierarchyWindow();
	m_ContentBrowser = &m_Editor->GetContentBrowser();
	m_DebugLogWindow = &m_Editor->GetDebugLogWindow();
	m_SystemLogWindow = &m_Editor->GetSystemLogWindow();
}

void MainMenuBar::RenderFileMenu() {

	if (ImGui::BeginMenu("File")) {

		ImGui::MenuItem("Test1", "CTRL + D");
		ImGui::MenuItem("Test2", "CTRL + D");
		ImGui::MenuItem("Test3", "CTRL + D");
		ImGui::MenuItem("Test4", "CTRL + D");


		ImGui::EndMenu();
	}
}
void MainMenuBar::RenderWindowMenu() {

	if (ImGui::BeginMenu("Window")) {

		ImGui::MenuItem("Details", "CTRL + D", &m_DetailsWindow->GetStateRef());
		ImGui::MenuItem("Heirarchy", "CTRL + D", &m_HierarchyWindow->GetStateRef());
		if (ImGui::MenuItem("Content Browser", "CTRL + D", &m_ContentBrowser->GetStateRef()))
			m_ContentBrowser->ResetWindow();

		if (ImGui::BeginMenu("Logs")) {
			if (ImGui::MenuItem("Debug Log", "CTRL + D", &m_DebugLogWindow->GetStateRef()))
				m_DebugLogWindow->ResetWindow();
			if (ImGui::MenuItem("System Log", "CTRL + D", &m_SystemLogWindow->GetStateRef()))
				m_SystemLogWindow->ResetWindow();

			ImGui::EndMenu();
		}


		ImGui::EndMenu();
	}
}
void MainMenuBar::RenderAboutMenu() {

	ImGuiWindowFlags AboutMenuFlags = 0;
	if (ImGui::MenuItem("About")) {
		ImGui::OpenPopup("AboutMenu");
		AboutMenuFlags |= ImGuiWindowFlags_NoTitleBar;
		AboutMenuFlags |= ImGuiWindowFlags_NoResize;
		AboutMenuFlags |= ImGuiWindowFlags_NoMove;
		AboutMenuFlags |= ImGuiWindowFlags_NoScrollbar;
	}
	if (ImGui::BeginPopupModal("AboutMenu", nullptr, AboutMenuFlags)) {

		//NOTE: Just get the viewport ptr once at the start.... do the same for hte other classes too
		ImGui::SetWindowPos(ImVec2((m_ImGuiViewport->Size.x / 2) - 200.0f, (m_ImGuiViewport->Size.y / 2) - 100.0f));
		ImGui::SetWindowSize(ImVec2(400.0f, 200.0f));

		m_Editor->AddSpacings(5);
		ImGui::SameLine(200.0f - ImGui::CalcTextSize("Primrose").x / 2);
		ImGui::Text("Primrose");
		m_Editor->AddSpacings(5);
		m_Editor->AddSeparators(1);

		m_Editor->AddSpacings(5);
		ImGui::SameLine(200.0f - ImGui::CalcTextSize("This engine is made by Mohammed Tamim.").x / 2);
		ImGui::Text("This engine is made by Mohammed Tamim.");

		ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x / 2) - 50.0f, ImGui::GetWindowSize().y - 50.0f)); // x - 50 is button width
		if (ImGui::Button("##CloseAboutMenu", ImVec2(100.0f, 20.0f)))
			ImGui::CloseCurrentPopup();


		ImVec2 TextPosition = { ImGui::GetWindowSize().x / 2 - 50.0f, ImGui::GetWindowSize().y - 50.0f };

		// -2 is a hardcoded number - Its fine since it scales with other elements
		TextPosition.x += ImGui::CalcTextSize("Close").x - 2;
		TextPosition.y += (ImGui::CalcTextSize("Close").y / 2) - 2;

		ImGui::SetCursorPos(TextPosition);
		ImGui::Text("Close");

		ImGui::EndPopup();
	}
}
