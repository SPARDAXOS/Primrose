#include "Editor/MaterialEditor.hpp"
#include "Systems/Core.hpp"
#include "Systems/Editor.hpp"

#include "Editor/SelectionWindows.hpp"

MaterialEditor::MaterialEditor(Core& core, Editor& editor, SelectionWindows& selectionWindows) noexcept
	: m_Core(&core), m_Editor(&editor), m_SelectionWindows(&selectionWindows)
{
	CalculateSectionNamesSizes();
}

void MaterialEditor::Render() {

	if (!m_Opened || m_Target == nullptr)
		return;

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_NoSavedSettings;


	if (m_WindowSizeReset) {
		m_WindowSizeReset = false;
		m_CurrentWindowSize = ImVec2(m_Editor->GetGUIViewport()->Size.x * 0.2f, m_Editor->GetGUIViewport()->Size.y * 0.6f); //Default Window Size
		ImGui::SetNextWindowSize(m_CurrentWindowSize);
		ImGui::SetNextWindowPos(m_Editor->GetUniqueScreenCenterPoint(m_CurrentWindowSize));
	}

	if (ImGui::Begin("Material Editor", &m_Opened, Flags)) {
		m_Editor->CheckForHoveredWindows();

		//TODO: Separate style options for each editor section. In this case, the material editor.
		//Style for texture selectors
		//Move this to function SetupStyle();
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.2f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.3f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.4f, 0.0f, 0.0f, 1.0f));


		//NOTE: The way everything is aligned is that each section starts with texture type that also works as category separator
		//-The name of the category works as distance from the start of the line so all elements under that category will have the same 
		//-Distance from start of the line in their position calculation.

		//TODO: When it comes to the texture selectors, break them into a reusable function and move a lot of the const variables here to member variables.
		//Also rework this method thing between here and the sprite renderer one final time!

				
		//TODO: When deleting materials. Check if it is selected!. Make general function for that stuff! 
		//-It checks if it is a selected asset/material/gameobject then does the appropriate thing like closing an editor or something.



		const ImVec2 NoneTextSize = ImGui::CalcTextSize("None");
		const float LineStartOffsetX = m_CurrentWindowSize.x * 0.05f;
		const float SelectorBoxWidth = (m_CurrentWindowSize.x * 0.2f);
		const float SelectorBoxHeight = 20.0f;
		const float NameToSelectorBoxPadding = m_CurrentWindowSize.x * 0.05f; 

		ImVec2 CurrentTextureTypeTextSize;
		ImVec2 CurrentSelectorBoxSize;
		ImVec2 CurrentTextureNameTextSize;

		std::string TextureName;

		float CurrentDistanceToSelectorBox;
		float CurrentLineHeight;
		
		//////////
		//Diffuse
		//////////
		// IMPORTANT NOTE: This is the way to create such input fields going forward.
		//Texture
		//CurrentLineHeight = ImGui::GetCursorPos().y;
		//CurrentTextureTypeTextSize = ImGui::CalcTextSize("Diffuse");
		//CurrentDistanceToSelectorBox = LineStartOffsetX + CurrentTextureTypeTextSize.x + NameToSelectorBoxPadding;
		//ImGui::SetCursorPos(ImVec2(LineStartOffsetX, CurrentLineHeight)); //Offset from line start
		//ImGui::Text("Diffuse");
		//ImGui::SameLine(CurrentDistanceToSelectorBox); 
		//ImGui::SetCursorPosY(CurrentLineHeight - 2); //It needs to be offset by -2 to align properly for some reason..

		////Value Name Text Calculation
		//if (m_Target->m_Diffuse != nullptr) {
		//	TextureName = m_Target->m_Diffuse->GetNameWithoutExtension();
		//	CurrentTextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		//}
		//else {
		//	TextureName = "None";
		//	CurrentTextureNameTextSize = NoneTextSize;
		//}

		////Selector Box
		//CurrentSelectorBoxSize = ImVec2(CurrentTextureNameTextSize.x + SelectorBoxWidth, SelectorBoxHeight);
		//if (ImGui::Button("##DiffuseSelector", CurrentSelectorBoxSize)) {
		//	m_SelectionWindows->SetSpriteSelectorWindowState(true);
		//	m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Diffuse);
		//}

		////Value Name Text
		//ImGui::SameLine(CurrentDistanceToSelectorBox + (CurrentSelectorBoxSize.x / 2) - (CurrentTextureNameTextSize.x / 2));
		//ImGui::SetCursorPosY(CurrentLineHeight - 2);
		//ImGui::Text(TextureName.data());


		SetupDiffuseSection();


		//////////
		//Ambient
		//////////
		//Texture
		CurrentLineHeight = ImGui::GetCursorPos().y;
		CurrentTextureTypeTextSize = ImGui::CalcTextSize("Ambient");
		CurrentDistanceToSelectorBox = LineStartOffsetX + CurrentTextureTypeTextSize.x + NameToSelectorBoxPadding;
		ImGui::SetCursorPos(ImVec2(LineStartOffsetX, CurrentLineHeight));
		ImGui::Text("Ambient");
		ImGui::SameLine(CurrentDistanceToSelectorBox);
		ImGui::SetCursorPosY(CurrentLineHeight - 2);

		//Value Name Text Calculation
		if (m_Target->m_Ambient != nullptr) {
			TextureName = m_Target->m_Ambient->GetNameWithoutExtension();
			CurrentTextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			CurrentTextureNameTextSize = NoneTextSize;
		}

		//Selector Box
		CurrentSelectorBoxSize = ImVec2(CurrentTextureNameTextSize.x + SelectorBoxWidth, SelectorBoxHeight);
		if (ImGui::Button("##AmbientSelector", CurrentSelectorBoxSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Ambient);
		}

		//Value Name Text
		ImGui::SameLine(CurrentDistanceToSelectorBox + (CurrentSelectorBoxSize.x / 2) - (CurrentTextureNameTextSize.x / 2));
		ImGui::SetCursorPosY(CurrentLineHeight - 2);
		ImGui::Text(TextureName.data());


		//Strength
		ImGui::SetCursorPos(ImVec2(LineStartOffsetX, ImGui::GetCursorPos().y));
		ImGui::Text("Strength");
		ImGui::SameLine(CurrentDistanceToSelectorBox); //Aligned with the current sprite selector box
		ImGui::SetNextItemWidth(CurrentTextureNameTextSize.x + SelectorBoxWidth);
		ImGui::InputFloat("##AmbientStrength", &m_Target->m_AmbientStrength);

		ImGui::Separator();


		//////////
		//Specular
		//////////

		//Texture
		CurrentLineHeight = ImGui::GetCursorPos().y;
		CurrentTextureTypeTextSize = ImGui::CalcTextSize("Specular");
		CurrentDistanceToSelectorBox = LineStartOffsetX + CurrentTextureTypeTextSize.x + NameToSelectorBoxPadding;
		ImGui::SetCursorPos(ImVec2(LineStartOffsetX, CurrentLineHeight));
		ImGui::Text("Specular");
		ImGui::SameLine(CurrentDistanceToSelectorBox);
		ImGui::SetCursorPosY(CurrentLineHeight - 2);

		//Value Name Text Calculation
		if (m_Target->m_Specular != nullptr) {
			TextureName = m_Target->m_Specular->GetNameWithoutExtension();
			CurrentTextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			CurrentTextureNameTextSize = NoneTextSize;
		}

		//Selector Box
		CurrentSelectorBoxSize = ImVec2(CurrentTextureNameTextSize.x + SelectorBoxWidth, SelectorBoxHeight);
		if (ImGui::Button(TextureName.data(), CurrentSelectorBoxSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Specular);
		}

		//Value Name Text
		ImGui::SameLine(CurrentDistanceToSelectorBox + (CurrentSelectorBoxSize.x / 2) - (CurrentTextureNameTextSize.x / 2));
		ImGui::SetCursorPosY(CurrentLineHeight - 2);
		ImGui::Text(TextureName.data());


		//Strength
		ImGui::SetCursorPos(ImVec2(LineStartOffsetX, ImGui::GetCursorPos().y));
		ImGui::Text("Strength");
		ImGui::SameLine(CurrentDistanceToSelectorBox);
		ImGui::SetNextItemWidth(CurrentTextureNameTextSize.x + SelectorBoxWidth);
		ImGui::InputFloat("##SpecularStrength", &m_Target->m_SpecularStrength);

		//Shininess
		ImGui::SetCursorPos(ImVec2(LineStartOffsetX, ImGui::GetCursorPos().y));
		ImGui::Text("Shininess");
		ImGui::SameLine(CurrentDistanceToSelectorBox);
		ImGui::SetNextItemWidth(CurrentTextureNameTextSize.x + SelectorBoxWidth);
		ImGui::InputInt("##SpecularShininess", &m_Target->m_SpecularShininess);
		ImGui::Separator();


		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);

		m_CurrentWindowSize = ImGui::GetWindowSize();
	}
	ImGui::End();
}

void MaterialEditor::SetupDiffuseSection() {

	SetupSelectorBox(SelectorBoxType::DIFFUSE);
	ImGui::Separator();
}
void MaterialEditor::SetupAmbientSection() {


}
void MaterialEditor::SetupSpecularSection() {


}
	 
void MaterialEditor::SetupSelectorBox(SelectorBoxType type) {

	//Move to member variables.
	const ImVec2 NoneTextSize = ImGui::CalcTextSize("None");
	const float LineStartOffsetX = m_CurrentWindowSize.x * 0.05f;
	const float SelectorBoxWidth = (m_CurrentWindowSize.x * 0.2f);
	const float SelectorBoxHeight = 20.0f;
	const float NameToSelectorBoxPadding = m_CurrentWindowSize.x * 0.05f;


	ImVec2 SelectorBoxSize;
	ImVec2 TextureNameTextSize;
	std::string TextureName;

	float DistanceToSelectorBox = 0.0f;
	const float CurrentLineHeight = ImGui::GetCursorPos().y;
	ImGui::SetCursorPos(ImVec2(LineStartOffsetX, CurrentLineHeight)); //Offset from line start

	switch (type) {
	case MaterialEditor::SelectorBoxType::DIFFUSE: {
		DistanceToSelectorBox = LineStartOffsetX + m_DiffuseSectionOffset + NameToSelectorBoxPadding;
		ImGui::Text("Diffuse");
	} break;
	case MaterialEditor::SelectorBoxType::AMBIENT: {
		DistanceToSelectorBox = LineStartOffsetX + m_AmbientSectionOffset + NameToSelectorBoxPadding;
		ImGui::Text("Ambient");
	} break;
	case MaterialEditor::SelectorBoxType::SPECULAR: {
		DistanceToSelectorBox = LineStartOffsetX + m_SpecularSectionOffset + NameToSelectorBoxPadding;
		ImGui::Text("Specular");
	} break;
	default:
		break;
	}

	ImGui::SameLine(DistanceToSelectorBox);
	ImGui::SetCursorPosY(CurrentLineHeight - 2); //It needs to be offset by -2 to align properly for some reason..


	switch (type) {
	case MaterialEditor::SelectorBoxType::DIFFUSE: {
		if (m_Target->m_Diffuse != nullptr) {
			TextureName = m_Target->m_Diffuse->GetNameWithoutExtension();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = NoneTextSize;
		}
	} break;
	case MaterialEditor::SelectorBoxType::AMBIENT: {
		if (m_Target->m_Ambient != nullptr) {
			TextureName = m_Target->m_Ambient->GetNameWithoutExtension();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = NoneTextSize;
		}
	} break;
	case MaterialEditor::SelectorBoxType::SPECULAR: {
		if (m_Target->m_Specular != nullptr) {
			TextureName = m_Target->m_Specular->GetNameWithoutExtension();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = NoneTextSize;
		}
	} break;
	default:
		break;
	}


	SelectorBoxSize = ImVec2(TextureNameTextSize.x + SelectorBoxWidth, SelectorBoxHeight);
	switch (type) {
	case MaterialEditor::SelectorBoxType::DIFFUSE: {
		if (ImGui::Button("##DiffuseSelector", SelectorBoxSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Diffuse);
		}
	} break;
	case MaterialEditor::SelectorBoxType::AMBIENT: {
		if (ImGui::Button("##AmbientSelector", SelectorBoxSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Ambient);
		}
	} break;
	case MaterialEditor::SelectorBoxType::SPECULAR: {
		if (ImGui::Button("##SpecularSelector", SelectorBoxSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Specular);
		}
	}break;
	default:
		break;
	}

	ImGui::SameLine(DistanceToSelectorBox + (SelectorBoxSize.x / 2) - (TextureNameTextSize.x / 2));
	ImGui::SetCursorPosY(CurrentLineHeight - 2);
	ImGui::Text(TextureName.data());
}
void MaterialEditor::CalculateSectionNamesSizes() {

	m_DiffuseSectionOffset = ImGui::CalcTextSize("Diffuse").x;
	m_AmbientSectionOffset = ImGui::CalcTextSize("Ambient").x;
	m_SpecularSectionOffset = ImGui::CalcTextSize("Specular").x;
}