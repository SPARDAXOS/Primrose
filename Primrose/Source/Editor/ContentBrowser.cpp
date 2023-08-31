#include "Editor/ContentBrowser.hpp"
#include "Systems/Core.hpp"
#include "Systems/Editor.hpp"
#include "Systems/AssetManager.hpp"
#include "Systems/TextureStorage.hpp"


ContentBrowser::ContentBrowser(Core& core, Editor& editor) noexcept
	: m_Core(&core), m_Editor(&editor)
{
	m_AssetManager = core.GetAssetManager();
	m_TextureStorage = core.GetTextureStorage();
}

void ContentBrowser::Update() {

	if (IsContentBrowserWindowHovered() && !ImGui::IsAnyItemHovered()) {

		if (m_SelectedContentElement != nullptr && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			m_SelectedContentElement = nullptr;

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
			m_Core->ErrorLog("Released");
			//Short afterimage when deleting!
			m_OpenContentBrowserEditMenu = true;
		}
	}
}
void ContentBrowser::Render() {

	if (!m_Opened)
		return;

	CheckViewportChanges();
	CheckWindowsChanges();
	if (m_ResetWindow)
		UpdateDockData();

	RenderDirectoryExplorer();
	RenderContentBrowser();

	if (m_ResetWindow)
		m_ResetWindow = false;
}
void ContentBrowser::Init() {

	//All asset icons 
	//Unknown asset
	//Error asset

	m_ImGuiViewport = m_Editor->GetGUIViewport();
	m_DebugLogWindow = m_Editor->GetDebugLogWindow();
	m_SystemLogWindow = m_Editor->GetSystemLogWindow();
	m_DetailsWindow = m_Editor->GetDetailsWindow();


	if (!m_TextureStorage->GetEditorTexture2DByName("Folder.png", m_FolderTexture))
		m_Core->SystemLog("Failed to save reference to engine texture [Folder]");

	if (!m_TextureStorage->GetEditorTexture2DByName("MaterialAsset.png", m_MaterialAssetTexture))
		m_Core->SystemLog("Failed to save reference to engine texture [Folder]");
}

void ContentBrowser::RenderContentBrowser() {

	m_ContentBrowserWindowHovered = false;

	bool m_IsOtherContentWindowOpened = false;
	m_IsOtherContentWindowOpened |= m_DebugLogWindow->GetState();
	m_IsOtherContentWindowOpened |= m_SystemLogWindow->GetState();


	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_NoSavedSettings;
	Flags |= ImGuiWindowFlags_NoMove;


	//IMPORTANT NOTE: When it comes to the text offeset bug, check the alignment on the text by PushStyleVar(). update: that didnt do it i think.
	if (m_ResetWindow) {
		ImGui::SetNextWindowSize(m_ContentBrowserWindowDockSize);
		ImGui::SetNextWindowPos(m_ContentBrowserWindowDockPosition);
	}
	if (ImGui::Begin("Content Browser", nullptr, Flags)) {
		m_Editor->CheckForHoveredWindows();
		if (ImGui::IsWindowHovered())
			m_ContentBrowserWindowHovered = true;

		if (m_SelectedDirectory != nullptr) {
			m_Editor->AddSpacings(5);
			SetupStyle();
			NewContentBrowserFrame();
			UpdateContentBrowserFolderEntries();
			if (!m_FolderEntryOpened)
				UpdateContentBrowserAssetEntries();

			FlushContentTexts();
			ClearStyle();
		}

		m_ContentBrowserWindowCurrentSize = ImGui::GetWindowSize();
		m_ContentBrowserWindowCurrentPosition = ImGui::GetWindowPos();
	}
	ImGui::End();

	UpdateContentBrowserMenu();
}
void ContentBrowser::RenderDirectoryExplorer() {

	bool m_IsOtherContentWindowOpened = false;
	m_IsOtherContentWindowOpened |= m_DebugLogWindow->GetState();
	m_IsOtherContentWindowOpened |= m_SystemLogWindow->GetState();


	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_NoSavedSettings;
	Flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
	Flags |= ImGuiWindowFlags_NoMove;
	Flags |= ImGuiWindowFlags_MenuBar;


	if (m_ResetWindow) {
		ImGui::SetNextWindowSize(m_DirectoryExplorerWindowDockSize);
		ImGui::SetNextWindowPos(m_DirectoryExplorerWindowDockPosition);
	}
	if (ImGui::Begin("Directory Explorer", &m_Opened, Flags)) {
		m_Editor->CheckForHoveredWindows();

		if (ImGui::BeginMenuBar()) {
			//Editor Filter
			ImGui::SetWindowFontScale(0.95f);
			const auto TextSize = ImGui::CalcTextSize("Editor");
			ImGui::Selectable("Editor", &m_DirectoryExplorerEditorFilter, 0, TextSize);
			ImGui::SetWindowFontScale(1.0f);


			ImGui::EndMenuBar();
		}



		auto ProjectRoot = m_AssetManager->GetProjectRoot();
		if (ProjectRoot == nullptr) {
			m_Core->SystemLog("m_AssetManagerReference->GetProjectRoot() returned null! - RenderDirectoryExplorer()");
			return;
		}
		if (m_SelectedDirectory == nullptr) //Set default directory if not set
			m_SelectedDirectory = ProjectRoot;
		AddFileExplorerEntry(ProjectRoot);

		//TODO: Could be enabled by filter
		//TODO: Do something about the assets then cause they will not be loaded from the correct texture storage for the editor. Content Browser!

		if (m_DirectoryExplorerEditorFilter) {
			auto EditorRoot = m_AssetManager->GetEditorRoot();
			if (EditorRoot == nullptr) {
				m_Core->SystemLog("m_AssetManagerReference->GetEditorRoot() returned null! - RenderDirectoryExplorer()");
				return;
			}
			AddFileExplorerEntry(EditorRoot);
		}

		m_DirectoryExplorerWindowCurrentSize = ImGui::GetWindowSize();
		m_DirectoryExplorerWindowCurrentPosition = ImGui::GetWindowPos();
	}

	ImGui::End();
}

void ContentBrowser::NewContentBrowserFrame() noexcept {

	//TODO: Maybe move these in and try to minimize these weird variables being member variables
	m_ContentElementCursor = 0.0f;
	m_ContentLineElementsCount = 0;
	m_FolderEntryOpened = false;
}
void ContentBrowser::UpdateContentBrowserMenu() {

	//Shit name for the function. This is the edit menu for the content browser.
	//Open Popup
	if (m_OpenContentBrowserEditMenu) {
		if (!ImGui::IsPopupOpen("##ContentBrowserMenu"))
			ImGui::OpenPopup("##ContentBrowserMenu");
		m_OpenContentBrowserEditMenu = false;
	}

	//Render Popup
	if (ImGui::BeginPopup("##ContentBrowserMenu")) {

		ImGui::SeparatorText("Add new...");
		if (ImGui::MenuItem("New folder")) {
			m_AssetManager->CreateNewFolder(*m_SelectedDirectory);
		}
		else if (ImGui::MenuItem("Material")) {
			m_AssetManager->CreateAsset(AssetType::MATERIAL, *m_SelectedDirectory);
		}

		ImGui::EndPopup();
	}
}
void ContentBrowser::UpdateContentBrowserFolderEntries() {

	//TODO: Implement some countermeasure in case the texture was not found
	//Bind texture
	void* TextureID = nullptr;
	if (m_FolderTexture) {
		m_FolderTexture->Bind();
		TextureID = (void*)(intptr_t)(m_FolderTexture->GetID());
	}

	for (auto& Folder : m_SelectedDirectory->m_Folders) {

		std::string ID = "##" + Folder->m_Name;
		bool AppliedStyle = false;

		//Calculate position and check if new line is necessary
		m_ContentElementCursor = (m_ContentBrowserElementPadding * (m_ContentLineElementsCount + 1)) + m_ContentBrowserElementSize.x * m_ContentLineElementsCount;
		if (m_ContentElementCursor + m_ContentBrowserElementSize.x >= m_ContentBrowserWindowCurrentSize.x) {
			FlushContentTexts();
			m_ContentElementCursor = m_ContentBrowserElementPadding;
			m_ContentLineElementsCount = 0; //This means its a new line!
		}

		//Apply selected style
		if (m_SelectedContentElement == Folder) {
			AppliedStyle = true;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.6f));
		}


		//Render content
		ImGui::SameLine(m_ContentElementCursor);
		if (ImGui::ImageButton(ID.data(), TextureID, m_ContentBrowserElementSize)) {
			if (m_SelectedContentElement == Folder) {
				m_SelectedDirectory = Folder;
				m_SelectedContentElement = nullptr;
				ImGui::PopStyleColor();
				m_FolderEntryOpened = true;
				break;
			}
			else
				m_SelectedContentElement = Folder;
		}

		//Pop selected style
		if (AppliedStyle)
			ImGui::PopStyleColor();

		//Book keeping
		m_QueuedContentTexts.push_back(Folder->m_Name);
		m_ContentLineElementsCount++;


		//Edit Menu - IsItemHovered() checks last submitted item so this needs to be afterwards
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
			if (m_SelectedContentElement != Folder)
				m_SelectedContentElement = Folder;

			m_FolderEditMenuTarget = Folder;
			ImGui::OpenPopup(m_FolderEditMenuTarget->m_Name.data());
		}

		//NOTE: I put it here since once the asset is delete in this func, I shouldnt do anything else with it!
		if (m_FolderEditMenuTarget == Folder) {

			//TODO: This could be moved into its own function and the size could be made into a member variable
			ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));
			if (ImGui::BeginPopup(m_FolderEditMenuTarget->m_Name.data())) {

				ImGui::SeparatorText(std::string("Edit: " + Folder->m_Name).data());
				if (ImGui::MenuItem("Rename")) {

				}
				if (ImGui::MenuItem("Delete"))
					m_AssetManager->RemoveDirectory(*Folder);

				ImGui::EndPopup();
			}
		}
	}

	//Unbind texture
	if (m_FolderTexture)
		m_FolderTexture->Unbind();
}
void ContentBrowser::UpdateContentBrowserAssetEntries() {

	for (auto& Asset : m_SelectedDirectory->m_Assets) {

		//TODO: Deal with in case it wasnt found! - Doesnt the function do that?
		void* TextureID = nullptr;
		const Texture2D* IconTexture = m_Editor->GetIconTexture(*Asset); //GetIconTexture Should probably be here then.
		if (IconTexture != nullptr) {
			TextureID = (void*)(intptr_t)(IconTexture->GetID());
			m_TextureStorage->SetActiveTextureUnit(TextureUnit::TEXTURE0);
			IconTexture->Bind();
		}


		std::string ID = "##" + Asset->m_Name;
		bool AppliedStyle = false;


		//Calculate position and check if new line is necessary
		m_ContentElementCursor = (m_ContentBrowserElementPadding * (m_ContentLineElementsCount + 1)) + m_ContentBrowserElementSize.x * m_ContentLineElementsCount;
		if (m_ContentElementCursor + m_ContentBrowserElementSize.x >= m_ContentBrowserWindowCurrentSize.x) {

			FlushContentTexts();
			m_ContentElementCursor = m_ContentBrowserElementPadding;
			m_ContentLineElementsCount = 0; //This means its a new line!
		}

		//Apply selected style
		if (m_SelectedContentElement == Asset) {
			AppliedStyle = true;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.6f));
		}

		//Render content
		ImGui::SameLine(m_ContentElementCursor);
		if (ImGui::ImageButton(ID.data(), TextureID, m_ContentBrowserElementSize)) {

			//Select
			if (m_SelectedContentElement == Asset) {
				m_SelectedContentElement = nullptr;
				m_Editor->OpenAsset(*Asset);
			}
			else
				m_SelectedContentElement = Asset;
		}


		//Pop selected style
		if (AppliedStyle)
			ImGui::PopStyleColor();

		m_QueuedContentTexts.push_back(Asset->m_Name);
		m_ContentLineElementsCount++;

		if (IconTexture != nullptr)
			IconTexture->Unbind();

		//Edit Menu - IsItemHovered() checks last submitted item so this needs to be afterwards
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
			if (m_SelectedContentElement != Asset)
				m_SelectedContentElement = Asset;

			m_AssetEditMenuTarget = Asset;
			ImGui::OpenPopup(m_AssetEditMenuTarget->m_Name.data());
		}

		//NOTE: I put it here since once the asset is delete in this func, I shouldnt do anything else with it!
		//I dont need the bool
		if (m_AssetEditMenuTarget == Asset) {

			//TODO: This could be moved into its own function and the size could be made into a member variable
			ImGui::SetNextWindowSize(ImVec2(200.0f, 100.0f));
			if (ImGui::BeginPopup(m_AssetEditMenuTarget->m_Name.data())) {

				ImGui::SeparatorText(std::string("Edit: " + Asset->m_Name).data());
				if (ImGui::MenuItem("Rename")) {

				}
				if (ImGui::MenuItem("Delete"))
					m_AssetManager->RemoveAsset(*Asset);

				//TODO: Clean up to avoid after image

				ImGui::EndPopup();
			}
		}
	}
}
void ContentBrowser::FlushContentTexts() {

	if (m_QueuedContentTexts.size() == 0)
		return;

	m_Editor->AddSpacings(1);
	float CurrentX = 0.0f;

	for (uint32 Index = 0; Index < m_QueuedContentTexts.size(); Index++) {

		std::string ElementName = m_QueuedContentTexts.at(Index);

		ImVec2 TextSize = ImGui::CalcTextSize(ElementName.data());
		CurrentX = (m_ContentBrowserElementPadding * (Index + 1)) + m_ContentBrowserElementSize.x * Index; //Same pos as content
		CurrentX += m_ContentBrowserElementSize.x / 2.0f; //Shift it by button half button size
		CurrentX -= TextSize.x / 2.0f; //Shift it back by text half width
		//TODO: Fix the positioning, it is slightly off still.

		ImGui::SameLine(CurrentX);
		ImGui::Text(ElementName.data());
	}

	m_QueuedContentTexts.clear();
	m_Editor->AddSpacings(3);
}

void ContentBrowser::AddFileExplorerEntry(Directory* entry) {

	if (entry == nullptr)
		return;

	ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
	bool IsLeaf = false;
	if (entry->m_Folders.size() == 0) {
		Flags |= ImGuiTreeNodeFlags_Leaf;
		IsLeaf = true;
	}

	if (entry == m_SelectedDirectory)
		Flags |= ImGuiTreeNodeFlags_Selected;

	if (ImGui::TreeNodeEx(entry->m_Path.filename().string().data(), Flags)) {

		if (ImGui::IsItemClicked()) {
			SetSelectedDirectory(*entry);
		}

		if (!IsLeaf) {
			for (auto& x : entry->m_Folders)
				AddFileExplorerEntry(x);
		}

		ImGui::TreePop();
	}

}

void ContentBrowser::SetupStyle() {

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
}
void ContentBrowser::ClearStyle() {

	//Make sure its the same amount of PushStyleColor() calls in SetupContentBrowserStyle();
	ImGui::PopStyleColor(3);
}

void ContentBrowser::UpdateDockData() noexcept {

	m_DirectoryExplorerWindowDockSize = ImVec2(m_ImGuiViewport->Size.x * 0.1f, m_ImGuiViewport->Size.y * 0.3f);

	//Requires m_DirectoryExplorerWindowDockSize.y to be updated first

	m_ContentBrowserWindowDockSize = ImVec2(m_ImGuiViewport->Size.x - m_DirectoryExplorerWindowDockSize.x - m_DetailsWindow->GetCurrentSize().x, m_DirectoryExplorerWindowDockSize.y);
	m_ContentBrowserWindowDockPosition = ImVec2(m_DirectoryExplorerWindowDockSize.x, m_ImGuiViewport->Size.y - m_DirectoryExplorerWindowDockSize.y);
	m_DirectoryExplorerWindowDockPosition = ImVec2(0.0f, m_ImGuiViewport->Size.y - m_DirectoryExplorerWindowDockSize.y);
}
void ContentBrowser::CheckViewportChanges() noexcept {
	if (m_ImGuiViewport->Size.x != m_LastViewportSize.x || m_ImGuiViewport->Size.y != m_LastViewportSize.y) {
		m_ResetWindow = true;
		m_LastViewportSize = m_ImGuiViewport->Size;
	}
}
void ContentBrowser::CheckWindowsChanges() noexcept {
	if (m_DetailsWindow->GetCurrentSize().x != m_LastDetailsWindowSize.x || m_DetailsWindow->GetCurrentSize().y != m_LastDetailsWindowSize.y) {
		m_ResetWindow = true;
		m_LastDetailsWindowSize = m_DetailsWindow->GetCurrentSize();
	}
}