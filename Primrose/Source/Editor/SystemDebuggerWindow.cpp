#include "Editor/SystemDebuggerWindow.hpp"

#include "Systems/Core.hpp"
#include "Systems/Editor.hpp"
#include "Tools/Logger.hpp" //idk about this being a tool..
#include "Systems/Time.hpp"


SystemDebuggerWindow::SystemDebuggerWindow(Core& core, Editor& editor) noexcept
	: m_Core(&core), m_Editor(&editor)
{
	m_Logger = core.GetLogger();
	m_Time = core.GetTime();
	m_ContentBrowser = editor.GetContentBrowser();
	m_DebuggerWindow = editor.GetDebuggerWindow();
}



void SystemDebuggerWindow::Render() {

	if (!m_Open)
		return;

	bool m_IsOtherContentWindowOpened = false;
	m_IsOtherContentWindowOpened |= m_ContentBrowser->GetState();
	m_IsOtherContentWindowOpened |= m_DebugLogOpened;

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_MenuBar;
	Flags |= ImGuiWindowFlags_NoSavedSettings;

	if (m_SystemLogWindowReset) {
		m_SystemLogWindowReset = false;

		if (!m_IsOtherContentWindowOpened) {
			ImGui::SetNextWindowPos(m_ContentBrowserWindowPosition);
			ImGui::SetNextWindowSize(m_ContentBrowserWindowSize);
		}
		else {
			ImGui::SetNextWindowSize(m_NewStandaloneWindowSize);
			ImGui::SetNextWindowPos(GetUniqueScreenCenterPoint(m_NewStandaloneWindowSize));
		}
	}

	if (ImGui::Begin("System Log", &m_Open, Flags)) {
		m_Editor->CheckForHoveredWindows();

		//NOTE: Change the namings in the logger class
		//-Debug should be the catagory and maybe just message or something instead of it
		//System is also far too general

		if (ImGui::BeginMenuBar()) {

			bool ClearSelected = false;
			const ImVec2 ClearSize = ImGui::CalcTextSize("Clear");
			const ImVec2 AutoScrollSize = ImGui::CalcTextSize("AutoScroll");
			if (ImGui::Selectable("Clear", &ClearSelected, 0, ClearSize))
				m_Logger->ClearSystemLog();
			if (ImGui::Selectable("AutoScroll", m_Logger->GetSystemLogAutoScroll(), 0, AutoScrollSize))
				m_Logger->ToggleSystemLogAutoScroll();



			ImGui::EndMenuBar();
		}
		if (m_Logger->GetSystemMessagesCount() > 0) {

			while (!m_Logger->IsSystemBufferLooped()) {

				const Message* NewMessage = m_Logger->GetNextSystemMessage();
				if (NewMessage == nullptr)
					continue;

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImGui::Text("[%s] [%llu] %s", m_Time->FormatTime(NewMessage->GetTimestamp()).data(), NewMessage->GetTick(), NewMessage->GetData().data());
				ImGui::PopStyleColor();
				m_Editor->AddSeparators(1);

				//TODO: Moving the scroll manually disables auto scroll
				//AutoScroll debug log
				if (ImGui::GetScrollY() <= ImGui::GetScrollMaxY() && m_Logger->GetSystemLogAutoScroll())
					ImGui::SetScrollHereY(1.0f);
			}
		}

	}
	ImGui::End();
}
void SystemDebuggerWindow::Init() {

}

