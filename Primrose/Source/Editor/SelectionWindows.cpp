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

	UpdateWindowPosition();
}
void SelectionWindows::Render() {

	RenderSpriteSelector();
	RenderMaterialSelector();
}


void SelectionWindows::UpdateSpriteSelectorEntries() {


	//NOTE: This applies to the content browser as well but this could be cleaned a lot more than this
	m_Editor->AddSpacings(5);
	m_Editor->SetupContentBrowserStyle();

	m_SpriteSelectorElementCursor = 0.0f;
	m_SpriteSelectorLineElementsCount = 0;

	for (auto& Texture : m_TextureStorage->GetTexture2DStorage()) {


		//Calculate element per line depending on window size
		//TODO: Deal with in case it wasnt found!
		if (Texture == nullptr) {
			//Error texture!
			continue;
		}

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

		if (Texture != nullptr)
			Texture->Unbind();
	}

	m_Editor->ClearContentBrowserStyle();
	FlushSpriteSelectorTexts();
}
void SelectionWindows::UpdateMaterialSelectorEntries() {

	//NOTE: This applies to the content browser as well but this could be cleaned a lot more than this

	m_Editor->AddSpacings(5);
	m_Editor->SetupContentBrowserStyle();

	m_MaterialSelectorElementCursor = 0.0f;
	m_MaterialSelectorLineElementsCount = 0;

	//TODO: Rework GetTextureIcon(). Now it is used as simply get a texture2D for a texture asset and ALSO for editor textures like materials.
	//-Editor textures are lot simpler to deal with and dont require asset info unlike the texture assets. So, it would make since to separate them and maybe
	//-Use GetTextureIcon() for getting only editor textures while use RequestTexture() for getting textures for texture assets.
	for (auto& Material : m_AssetManager->GetMaterialsStorage()) {


		//Calculate element per line depending on window size
		//TODO: Deal with in case it wasnt found!
		if (Material == nullptr) {
			//Error texture!
			continue;
		}
		
		void* TextureID = nullptr;
		Texture2D* MaterialAssetTexture = m_Editor->GetIconTexture(Material->GetAsset()); //Will always or should always return a texture even if it doesnt find the correct one.
		TextureID = (void*)(intptr_t)(MaterialAssetTexture->GetID());
		MaterialAssetTexture->Bind();

		std::string Name = MaterialAssetTexture->GetName().data();
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

		MaterialAssetTexture->Unbind();
	}

	m_Editor->ClearContentBrowserStyle();
	FlushMaterialSelectorTexts();
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
		//TODO: Fix the positioning, it is slightly off still.

		ImGui::SameLine(CurrentX);
		ImGui::Text(ElementName.data());
	}

	m_QueuedMaterialSelectorTexts.clear();
	m_Editor->AddSpacings(3);
}

void SelectionWindows::UpdateWindowPosition() {
	//Consider making this vars maybe?
	if (!m_SpriteSelectorOpened)
		m_SpriteSelectorWindowSize = ImVec2(m_Editor->GetGUIViewport()->Size.x * 0.2f, m_Editor->GetGUIViewport()->Size.y * 0.6f);

	if (!m_MaterialSelectorOpened)
		m_MaterialSelectorWindowSize = ImVec2(m_Editor->GetGUIViewport()->Size.x * 0.2f, m_Editor->GetGUIViewport()->Size.y * 0.6f);
}