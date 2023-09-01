#include "Editor/SelectionWindows.hpp"
#include "Systems/Core.hpp"
#include "Systems/Editor.hpp"
#include "Systems/TextureStorage.hpp"
#include "Systems/AssetManager.hpp"

SelectionWindows::SelectionWindows(Core& core, Editor& editor) noexcept
	:	m_Core(&core), m_Editor(&editor)
{
	m_TextureStorage = core.GetTextureStorage();
	m_AssetManager = core.GetAssetManager();
}

void SelectionWindows::Update() {

	CheckViewportChanges();
}
void SelectionWindows::Render() {

	RenderSpriteSelector();
	RenderMaterialSelector();
}
void SelectionWindows::Init() {

	m_ImGuiViewport = m_Editor->GetGUIViewport();


	if (!m_TextureStorage->GetEditorTexture2DByName("MissingTexture.png", m_MissingTexture))
		m_Core->SystemLog("SelectionWindows failed to save reference to engine texture [MissingTexture.png]");

	if (!m_TextureStorage->GetEditorTexture2DByName("MaterialAsset.png", m_MaterialAssetTexture))
		m_Core->SystemLog("SelectionWindows failed to save reference to engine texture [MaterialAsset.png]");

}

void SelectionWindows::RenderSpriteSelector() {

	if (!m_SpriteSelectorOpened || m_SpriteSelectorTarget == nullptr)
		return;

	ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;


	//Open Popup
	if (m_SpriteSelectorOpened) {
		if (!ImGui::IsPopupOpen("Sprite Selector")) {
			ImGui::SetNextWindowPos(ImGui::GetMousePos());
			ImGui::SetNextWindowSize(m_SpriteSelectorWindowSize);
			ImGui::OpenPopup("Sprite Selector");
		}
	}

	//Render Popup
	if (ImGui::BeginPopupModal("Sprite Selector", &m_SpriteSelectorOpened, Flags)) {
		m_Editor->CheckForHoveredWindows();
		UpdateSpriteSelectorEntries();

		m_SpriteSelectorWindowSize = ImGui::GetWindowSize(); //This method is great but it requires that i keep track of the bool and edit it manually.
		//-Which is not a problem. ???
		ImGui::EndPopup();
	}
	else {
		if (m_SpriteSelectorTarget != nullptr)
			m_SpriteSelectorTarget = nullptr;
		m_SpriteSelectorOpened = false;
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}
void SelectionWindows::RenderMaterialSelector() {

	if (!m_MaterialSelectorOpened || m_MaterialSelectorTarget == nullptr)
		return;

	ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5.0f);

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;


	//Open Popup
	if (m_MaterialSelectorOpened) {
		if (!ImGui::IsPopupOpen("Material Selector")) {
			ImGui::SetNextWindowPos(ImGui::GetMousePos());
			ImGui::SetNextWindowSize(m_MaterialSelectorWindowSize);
			ImGui::OpenPopup("Material Selector");
		}
	}

	//Render Popup
	if (ImGui::BeginPopupModal("Material Selector", &m_MaterialSelectorOpened, Flags)) {
		m_Editor->CheckForHoveredWindows();
		UpdateMaterialSelectorEntries();
		m_MaterialSelectorWindowSize = ImGui::GetWindowSize();
		ImGui::EndPopup();
	}
	else {
		if (m_MaterialSelectorTarget != nullptr)
			m_MaterialSelectorTarget = nullptr;
		m_MaterialSelectorOpened = false;
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
}


void SelectionWindows::UpdateSpriteSelectorEntries() {


	//NOTE: This applies to the content browser as well but this could be cleaned a lot more than this
	m_Editor->AddSpacings(5);
	SetupStyle();

	m_SpriteSelectorElementCursor = 0.0f;
	m_SpriteSelectorLineElementsCount = 0;

	for (auto& Texture : m_TextureStorage->GetTexture2DStorage()) {


		//Calculate element per line depending on window size
		//TODO: Deal with in case it wasnt found!
		if (Texture == nullptr) { //Again, possible but idk... just remove this?
			//Error texture!
			continue;
		}


		//Bind Texture
		void* TextureID = nullptr;
		TextureID = (void*)(intptr_t)(Texture->GetID());
		Texture->Bind();



		std::string Name = Texture->GetName().data();
		std::string ID = "##" + Name;
		bool AppliedStyle = false;



		//Calculate position and check if new line is necessary
		m_SpriteSelectorElementCursor = (m_SpriteSelectorElementPadding * (m_SpriteSelectorLineElementsCount + 1)) + m_SpriteSelectorElementSize.x * m_SpriteSelectorLineElementsCount;
		if (m_SpriteSelectorElementCursor + m_SpriteSelectorElementSize.x >= m_SpriteSelectorWindowSize.x) {
			FlushSpriteSelectorTexts();
			m_SpriteSelectorElementCursor = m_SpriteSelectorElementPadding;
			m_SpriteSelectorLineElementsCount = 0; //This means its a new line!
		}

		//Apply selected style
		if (m_SelectedSpriteSelectorElement == Texture) {
			AppliedStyle = true;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.6f));
		}

		//Render content
		ImGui::SameLine(m_SpriteSelectorElementCursor);
		if (ImGui::ImageButton(ID.data(), TextureID, m_SpriteSelectorElementSize)) {
			if (m_SelectedSpriteSelectorElement == Texture) {
				m_SelectedSpriteSelectorElement = nullptr;

				*m_SpriteSelectorTarget = Texture;
				m_SpriteSelectorTarget = nullptr;

				ImGui::CloseCurrentPopup();
			}
			else
				m_SelectedSpriteSelectorElement = Texture;
		}

		//Pop selected style
		if (AppliedStyle)
			ImGui::PopStyleColor();

		m_QueuedSpriteSelectorTexts.emplace_back(Name);
		m_SpriteSelectorLineElementsCount++;

		//Unbind Texture - If this was reached then texture is valid.
		Texture->Unbind();
	}

	ClearStyle();
	FlushSpriteSelectorTexts();
}
void SelectionWindows::UpdateMaterialSelectorEntries() {

	//NOTE: This applies to the content browser as well but this could be cleaned a lot more than this

	m_Editor->AddSpacings(5);
	SetupStyle();

	m_MaterialSelectorElementCursor = 0.0f;
	m_MaterialSelectorLineElementsCount = 0;


	//Bind texture
	void* TextureID = nullptr;
	if (m_MaterialAssetTexture) {
		m_MaterialAssetTexture->Bind();
		TextureID = (void*)(intptr_t)(m_MaterialAssetTexture->GetID());
	}
	else {
		m_MissingTexture->Bind();
		TextureID = (void*)(intptr_t)(m_MissingTexture->GetID());
	}

	for (auto& Material : m_AssetManager->GetMaterialsStorage()) {


		//Calculate element per line depending on window size
		//This is possible but idk...
		if (Material == nullptr) {
			continue; 
		}
		

		std::string Name = Material->GetAsset().m_Name.data();
		std::string ID = "##" + Name;
		bool AppliedStyle = false;

		//Calculate position and check if new line is necessary
		m_MaterialSelectorElementCursor = (m_MaterialSelectorElementPadding * (m_MaterialSelectorLineElementsCount + 1)) + m_MaterialSelectorElementSize.x * m_MaterialSelectorLineElementsCount;
		if (m_MaterialSelectorElementCursor + m_MaterialSelectorElementSize.x >= m_MaterialSelectorWindowSize.x) {
			FlushMaterialSelectorTexts();
			m_MaterialSelectorElementCursor = m_MaterialSelectorElementPadding;
			m_MaterialSelectorLineElementsCount = 0; //This means its a new line!
		}

		//Apply selected style
		if (m_SelectedMaterialSelectorElement == Material) {
			AppliedStyle = true;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.6f));
		}

		//Render content
		ImGui::SameLine(m_MaterialSelectorElementCursor);
		if (ImGui::ImageButton(ID.data(), TextureID, m_MaterialSelectorElementSize)) {
			if (m_SelectedMaterialSelectorElement == Material) {
				m_SelectedMaterialSelectorElement = nullptr;

				*m_MaterialSelectorTarget = Material;
				m_MaterialSelectorTarget = nullptr;

				ImGui::CloseCurrentPopup();
			}
			else
				m_SelectedMaterialSelectorElement = Material; //The fact that its void* already confused the life out of me
		}

		//Pop selected style
		if (AppliedStyle)
			ImGui::PopStyleColor();

		m_QueuedMaterialSelectorTexts.emplace_back(Name);
		m_MaterialSelectorLineElementsCount++;
	}

	//Unbind Texture
	if (m_MaterialAssetTexture)
		m_MaterialAssetTexture->Unbind();
	else
		m_MissingTexture->Unbind();

	ClearStyle();
	FlushMaterialSelectorTexts();
}

void SelectionWindows::FlushSpriteSelectorTexts() {

	if (m_QueuedSpriteSelectorTexts.size() == 0)
		return;

	m_Editor->AddSpacings(1);
	float CurrentX = 0.0f;

	for (uint32 Index = 0; Index < m_QueuedSpriteSelectorTexts.size(); Index++) {

		std::string ElementName = m_QueuedSpriteSelectorTexts.at(Index);

		ImVec2 TextSize = ImGui::CalcTextSize(ElementName.data());
		CurrentX = (m_SpriteSelectorElementPadding * (Index + 1)) + m_SpriteSelectorElementSize.x * Index; //Same pos as content
		CurrentX += m_SpriteSelectorElementSize.x / 2.0f; //Shift it by button half button size
		CurrentX -= TextSize.x / 2.0f; //Shift it back by text half width

		CurrentX += 5;
		//TODO: Fix the positioning, it is slightly off still.

		ImGui::SameLine(CurrentX);
		ImGui::Text(ElementName.data());
	}

	m_QueuedSpriteSelectorTexts.clear();
	m_Editor->AddSpacings(3);
}
void SelectionWindows::FlushMaterialSelectorTexts() {

	if (m_QueuedMaterialSelectorTexts.size() == 0)
		return;

	m_Editor->AddSpacings(1);
	float CurrentX = 0.0f;

	for (uint32 Index = 0; Index < m_QueuedMaterialSelectorTexts.size(); Index++) {

		std::string ElementName = m_QueuedMaterialSelectorTexts.at(Index);

		ImVec2 TextSize = ImGui::CalcTextSize(ElementName.data());
		CurrentX = (m_MaterialSelectorElementPadding * (Index + 1)) + m_MaterialSelectorElementSize.x * Index; //Same pos as content
		CurrentX += m_MaterialSelectorElementSize.x / 2.0f; //Shift it by button half button size
		CurrentX -= TextSize.x / 2.0f; //Shift it back by text half width
		CurrentX += 5;
		//TODO: Fix the positioning, it is slightly off still.

		ImGui::SameLine(CurrentX);
		ImGui::Text(ElementName.data());
	}

	m_QueuedMaterialSelectorTexts.clear();
	m_Editor->AddSpacings(3);
}

void SelectionWindows::CheckViewportChanges() {

	if (!m_SpriteSelectorOpened)
		m_SpriteSelectorWindowSize = ImVec2(m_ImGuiViewport->Size.x * 0.2f, m_ImGuiViewport->Size.y * 0.6f);

	if (!m_MaterialSelectorOpened)
		m_MaterialSelectorWindowSize = ImVec2(m_ImGuiViewport->Size.x * 0.2f, m_ImGuiViewport->Size.y * 0.6f);
}

void SelectionWindows::SetupStyle() {

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
}
void SelectionWindows::ClearStyle() {

	//Make sure its the same amount of PushStyleColor() calls in SetupContentBrowserStyle();
	ImGui::PopStyleColor(3);
}
