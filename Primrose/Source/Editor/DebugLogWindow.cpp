#include "Editor/DebugLogWindow.hpp"

#include "Systems/Core.hpp"
#include "Systems/Editor.hpp"
#include "Tools/Logger.hpp" //idk about this being a tool..
#include "Systems/Time.hpp"
#include "Systems/TextureStorage.hpp"

#include "Editor/ContentBrowser.hpp"
#include "Editor/SystemLogWindow.hpp"


DebugLogWindow::DebugLogWindow(Core& core, Editor& editor) noexcept
	:	m_Core(&core), m_Editor(&editor)
{
	m_Logger = core.GetLogger();
	m_TextureStorage = core.GetTextureStorage();
	m_Time = core.GetTime();
}

void DebugLogWindow::Render() {

	if (!m_Open)
		return;

	bool m_IsOtherContentWindowOpened = false;
	m_IsOtherContentWindowOpened |= m_ContentBrowser->GetState();
	m_IsOtherContentWindowOpened |= m_SystemLogWindow->GetState();

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_MenuBar;
	Flags |= ImGuiWindowFlags_NoSavedSettings;

	if (m_WindowReset) {
		m_WindowReset = false;

		if (!m_IsOtherContentWindowOpened) {
			ImGui::SetNextWindowPos(m_ContentBrowser->GetContentBrowserWindowDockPosition());
			ImGui::SetNextWindowSize(m_ContentBrowser->GetContentBrowserWindowDockSize());
		}
		else {
			ImGui::SetNextWindowSize(m_Editor->GetNewStandaloneWindowSize());
			ImGui::SetNextWindowPos(m_Editor->GetUniqueScreenCenterPoint(m_Editor->GetNewStandaloneWindowSize()));
		}
	}

	if (ImGui::Begin("Debug Log", &m_Open, Flags)) {
		m_Editor->CheckForHoveredWindows();

		//TODO: These could be broken down into functions
		//TODO: The auto scroll could be made its own resusable function
		if (ImGui::BeginMenuBar()) {

			//I guess it makes sense why they use static so much
			bool ClearSelected = false;
			const ImVec2 ClearSize = ImGui::CalcTextSize("Clear");
			const ImVec2 AutoScrollSize = ImGui::CalcTextSize("AutoScroll");
			if (ImGui::Selectable("Clear", &ClearSelected, 0, ClearSize)) {
				m_Logger->ClearDebugLog();
			}
			if (ImGui::Selectable("AutoScroll", m_Logger->GetDebugLogAutoScroll(), 0, AutoScrollSize)) {
				m_Logger->ToggleDebugLogAutoScroll();
			}

			//TODO: Implement a function that checks for an engine texture. 
			//-If not found then it returns an error texture so then i can be sure im getting something each time

			//TODO: DRY, This down here could be abstarcted away into a resusable function


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.4f, 0.4f, 0.4f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
			ImVec4 IconColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			//Texture
			void* TextureID = nullptr;
			if (m_ErrorTexture) {
				m_ErrorTexture->Bind();
				TextureID = (void*)(intptr_t)(m_ErrorTexture->GetID());
			}

			//Color
			if (!m_Logger->GetShowErrorMessages())
				IconColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);


			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 30.0f, ImGui::GetCursorPosY())); //Manual offset by +2
			if (ImGui::ImageButton("##ErrorLog", TextureID, ImVec2(15.0f, 15.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), IconColor))
				m_Logger->ToggleShowErrorMessages();
			if (m_ErrorTexture != nullptr)
				m_ErrorTexture->Unbind();

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 5.0f, ImGui::GetCursorPosY()));
			ImGui::Text("%d", m_Logger->GetErrorMessagesCount());
			IconColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


			//Texture
			if (m_WarningTexture) {
				m_WarningTexture->Bind();
				TextureID = (void*)(intptr_t)(m_WarningTexture->GetID());
			}

			//Color
			if (!m_Logger->GetShowWarningMessages())
				IconColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 80.0f, ImGui::GetCursorPosY())); //Manual offset by +2
			if (ImGui::ImageButton("##WarningLog", TextureID, ImVec2(15.0f, 15.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), IconColor))
				m_Logger->ToggleShowWarningMessages();
			if (m_WarningTexture != nullptr)
				m_WarningTexture->Unbind();

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 55.0f, ImGui::GetCursorPosY()));
			ImGui::Text("%d", m_Logger->GetWarningMessagesCount());
			IconColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


			//Texture
			if (m_DebugTexture) {
				m_DebugTexture->Bind();
				TextureID = (void*)(intptr_t)(m_DebugTexture->GetID());
			}

			//Color
			if (!m_Logger->GetShowDebugMessages())
				IconColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 130.0f, ImGui::GetCursorPosY())); //Manual offset by +2
			if (ImGui::ImageButton("##DebugLog", TextureID, ImVec2(15.0f, 15.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), IconColor))
				m_Logger->ToggleShowDebugMessages();
			if (m_DebugTexture != nullptr)
				m_DebugTexture->Unbind();

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 105.0f, ImGui::GetCursorPosY()));
			ImGui::Text("%d", m_Logger->GetDebugMessagesCount());
			IconColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();


			ImGui::EndMenuBar();
		}
		if (m_Logger->GetLoggedMessagesCount() > 0) {

			while (!m_Logger->IsLoggedBufferLooped()) {

				const Message* NewMessage = m_Logger->GetNextLoggedMessage();
				if (NewMessage == nullptr)
					continue;

				const MessageType Type = NewMessage->GetType();
				if (Type == MessageType::DEBUG && !m_Logger->GetShowDebugMessages())
					continue;
				if (Type == MessageType::WARNING && !m_Logger->GetShowWarningMessages())
					continue;
				if (Type == MessageType::ERROR && !m_Logger->GetShowErrorMessages())
					continue;
				if (Type == MessageType::SYSTEM) {
					m_Logger->ErrorLog("System type message was saved in the logged messages buffer");
					continue;
				}


				if (Type == MessageType::DEBUG)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				else if (Type == MessageType::WARNING)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.1f, 1.0f));
				else if (Type == MessageType::ERROR)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				else if (Type == MessageType::SYSTEM)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));

				//TODO: Add icons and numbers to show count of each message type
				//TODO: Add visibility of each icon that indicates if the filter is active or not
				//TODO: Add more info to messages like running time - Check what Unity does

				//NOTE: Separate window and buffer for system messages?
				//TODO: Switch all log functions back to const char* and simply construct 1 string at the func definition in logger to minimize copies. Emplace it even

				ImGui::Text("[%s] [%llu] %s", m_Time->FormatTime(NewMessage->GetTimestamp()).data(), NewMessage->GetTick(), NewMessage->GetData().data());

				ImGui::PopStyleColor();
				m_Editor->AddSeparators(1);

				//AutoScroll
				if (ImGui::GetScrollY() <= ImGui::GetScrollMaxY() && m_Logger->GetDebugLogAutoScroll())
					ImGui::SetScrollHereY(1.0f);
			}
		}
	}
	ImGui::End();

}

void DebugLogWindow::Init() {

	m_ContentBrowser = m_Editor->GetContentBrowser();
	m_SystemLogWindow = m_Editor->GetSystemLogWindow();

	if (!m_TextureStorage->GetEditorTexture2DByName("Debug.png", m_DebugTexture))
		m_Core->SystemLog("Failed to save reference to engine texture [Debug]");

	if (!m_TextureStorage->GetEditorTexture2DByName("Warning.png", m_WarningTexture))
		m_Core->SystemLog("Failed to save reference to engine texture [Warning]");

	if (!m_TextureStorage->GetEditorTexture2DByName("Error.png", m_ErrorTexture))
		m_Core->SystemLog("Failed to save reference to engine texture [Error]");
}