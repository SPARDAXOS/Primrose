#include "Editor/MaterialEditor.hpp"
#include "Systems/Core.hpp"
#include "Systems/Editor.hpp"

#include "Editor/SelectionWindows.hpp"

MaterialEditor::MaterialEditor(Core& core, Editor& editor, SelectionWindows& selectionWindows) noexcept
	: m_Core(&core), m_Editor(&editor), m_SelectionWindows(&selectionWindows)
{
}

void MaterialEditor::Update() {

	UpdateWindowPosition();
}

void MaterialEditor::Render() {

	if (!m_Opened || m_Target == nullptr)
		return;

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_NoSavedSettings;


	if (m_WindowSizeReset) {
		m_WindowSizeReset = false;
		ImGui::SetNextWindowSize(m_CurrentWindowSize);
		ImGui::SetNextWindowPos(m_Editor->GetUniqueScreenCenterPoint(m_CurrentWindowSize));
	}


	if (ImGui::Begin("Material Editor", &m_Opened, Flags)) {
		m_Editor->CheckForHoveredWindows();

		//Style for texture selectors
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.2f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.3f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.4f, 0.0f, 0.0f, 1.0f));

		ImVec2 TextureTypeTextSize;
		ImVec2 TextureNameTextSize;
		ImVec2 SpriteSelectorSize;
		std::string TextureName;
		ImVec2 NoneTextSize = ImGui::CalcTextSize("None");

		//TODO: When deleting materials. Check if it is selected!. Make general function for that stuff! 
		//-It checks if it is a selected asset/material/gameobject then does the appropriate thing like closing an editor or something.





		//	ImVec2 ValueNameSize;
		//	ImVec2 InputBoxSize;
		//	ImVec2 ValueNameTextPosition;
		//	
		//	//Sprite
		//	//Parameter Name
		//	ImGui::Text("Sprite");
		//	ElementTextSize = ImGui::CalcTextSize("Sprite");
		//	ImGui::SameLine(ElementTextSize.x + (m_DetailsWindowSize.x * 0.05f));
		//	
		//	//Value Text Calculation
		//	const Texture2D* Sprite = SelectedSpriteRenderer->GetSprite();
		//	if (Sprite != nullptr)
		//		ValueName = Sprite->GetName();
		//	else
		//		ValueName = "None";
		//	ValueNameSize = ImGui::CalcTextSize(ValueName.data());
		//	
		//	//Input Box
		//	InputBoxSize = ImVec2(m_DetailsWindowSize.x * 0.2f + ValueNameSize.x, 15.0f);
		//	if (ImGui::Button("##SpriteNameButton", InputBoxSize)) {
		//		m_SpriteSelectorOpened = true;
		//		SetSpriteSelectorTarget(SelectedSpriteRenderer->GetSpriteRef());
		//	}
		//	
		//	//Value Name Text
		//	ImGui::SameLine(ElementTextSize.x + (m_DetailsWindowSize.x * 0.05f));
		//	ValueNameTextPosition.x = (ElementTextSize.x + (m_DetailsWindowSize.x * 0.05f) + InputBoxSize.x / 2) - (ValueNameSize.x / 2);
		//	ValueNameTextPosition.y = ((ImGui::GetCursorPosY() - InputBoxSize.y / 2) + ValueNameSize.y / 2) - 2; //Offset by -2 to make it look better!
		//	ImGui::SetCursorPos(ValueNameTextPosition);
		//	ImGui::Text(ValueName.data());




		//////////
		//Diffuse
		//////////

		//Texture
		ImGui::SetCursorPos(ImVec2(m_CurrentWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		TextureTypeTextSize = ImGui::CalcTextSize("Diffuse");
		ImGui::Text("Diffuse");
		ImGui::SameLine(TextureTypeTextSize.x + (m_CurrentWindowSize.x * 0.1f));


		if (m_Target->m_Diffuse != nullptr) {
			TextureName = m_Target->m_Diffuse->GetName();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = NoneTextSize;
		}
		//Note Its the label! Its the same for all of them i think - They can all be None
		//Also rework this method thing between here and the sprite renderer one final time!
		SpriteSelectorSize = ImVec2(TextureNameTextSize.x + (m_CurrentWindowSize.x * 0.2f), 20.0f);
		if (ImGui::Button(TextureName.data(), SpriteSelectorSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Diffuse);
		}








		ImGui::Separator();


		//////////
		//Ambient
		//////////

		//Texture
		ImGui::SetCursorPos(ImVec2(m_CurrentWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		TextureTypeTextSize = ImGui::CalcTextSize("Ambient");
		ImGui::Text("Ambient");
		ImGui::SameLine(TextureTypeTextSize.x + (m_CurrentWindowSize.x * 0.1f));
		if (m_Target->m_Ambient != nullptr) {
			TextureName = m_Target->m_Ambient->GetName();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = NoneTextSize;
		}

		SpriteSelectorSize = ImVec2(TextureNameTextSize.x + (m_CurrentWindowSize.x * 0.2f), 20.0f);
		if (ImGui::Button(TextureName.data(), SpriteSelectorSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Ambient);
		}

		//Strength
		ImGui::SetCursorPos(ImVec2(m_CurrentWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		ImGui::Text("Strength");
		ImGui::SameLine(ImGui::CalcTextSize("Strength").x + (m_CurrentWindowSize.x * 0.1f));
		ImGui::SetNextItemWidth(TextureNameTextSize.x + (m_CurrentWindowSize.x * 0.2f));
		ImGui::InputFloat("##AmbientStrength", &m_Target->m_AmbientStrength);
		ImGui::Separator();


		//NOTE: Check what flags i get.


		//////////
		//Specular
		//////////

		//Texture
		ImGui::SetCursorPos(ImVec2(m_CurrentWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		TextureTypeTextSize = ImGui::CalcTextSize("Specular");
		ImGui::Text("Specular");
		ImGui::SameLine(TextureTypeTextSize.x + (m_CurrentWindowSize.x * 0.1f));
		if (m_Target->m_Specular != nullptr) {
			TextureName = m_Target->m_Specular->GetName();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = NoneTextSize;
		}


		SpriteSelectorSize = ImVec2(TextureNameTextSize.x + (m_CurrentWindowSize.x * 0.2f), 20.0f);
		if (ImGui::Button(TextureName.data(), SpriteSelectorSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Specular);
		}

		//Strength
		ImGui::SetCursorPos(ImVec2(m_CurrentWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		ImGui::Text("Strength");
		ImGui::SameLine(ImGui::CalcTextSize("Strength").x + (m_CurrentWindowSize.x * 0.1f));
		ImGui::SetNextItemWidth(TextureNameTextSize.x + (m_CurrentWindowSize.x * 0.2f));
		ImGui::InputFloat("##SpecularStrength", &m_Target->m_SpecularStrength);

		//Shininess
		ImGui::SetCursorPos(ImVec2(m_CurrentWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		ImGui::Text("Shininess");
		ImGui::SameLine(ImGui::CalcTextSize("Shininess").x + (m_CurrentWindowSize.x * 0.1f));
		ImGui::SetNextItemWidth(TextureNameTextSize.x + (m_CurrentWindowSize.x * 0.2f));
		ImGui::InputInt("##SpecularShininess", &m_Target->m_SpecularShininess);
		ImGui::Separator();


		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);

		m_CurrentWindowSize = ImGui::GetWindowSize();
	}
	ImGui::End();
}

void MaterialEditor::UpdateWindowPosition() {

	if (!m_Opened)
		m_CurrentWindowSize = ImVec2(m_Editor->GetGUIViewport()->Size.x * 0.2f, m_Editor->GetGUIViewport()->Size.y * 0.6f);
}