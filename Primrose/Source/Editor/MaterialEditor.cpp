#include "Editor/MaterialEditor.hpp"
#include "Systems/Core.hpp"
#include "Systems/Editor.hpp"

#include "Editor/SelectionWindows.hpp"

MaterialEditor::MaterialEditor(Core& core, Editor& editor) noexcept
	: m_Core(&core), m_Editor(&editor)
{
}

void MaterialEditor::Update() {

	CheckOffsets();
}
void MaterialEditor::Render() {

	if (!m_Opened || m_Target == nullptr)
		return;

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_NoSavedSettings;


	if (m_WindowSizeReset) {
		m_WindowSizeReset = false;
		m_CurrentWindowSize = ImVec2(m_ImGuiViewport->Size.x * 0.2f, m_ImGuiViewport->Size.y * 0.6f); //Default Window Size
		ImGui::SetNextWindowSize(m_CurrentWindowSize);
		ImGui::SetNextWindowPos(m_Editor->GetUniqueScreenCenterPoint(m_CurrentWindowSize));
	}

	if (ImGui::Begin("Material Editor", &m_Opened, Flags)) {
		m_Editor->CheckForHoveredWindows();



		//NOTE: The way everything is aligned is that each section starts with texture type that also works as category separator
		//-The name of the category works as distance from the start of the line so all elements under that category will have the same 
		//-Distance from start of the line in their position calculation.

		//TODO: When it comes to the texture selectors, break them into a reusable function and move a lot of the const variables here to member variables.
		//Also rework this method thing between here and the sprite renderer one final time!

				
		//TODO: When deleting materials. Check if it is selected!. Make general function for that stuff! 
		//-It checks if it is a selected asset/material/gameobject then does the appropriate thing like closing an editor or something.

		// IMPORTANT NOTE: This is the way to create such input fields going forward.
		ApplyStyle();
		NewFrame();
		SetupDiffuseSection();
		SetupAmbientSection();
		SetupSpecularSection();
		ClearStyle();

		m_CurrentWindowSize = ImGui::GetWindowSize();
	}
	ImGui::End();
}
void MaterialEditor::Init() {

	m_SelectionWindows = &m_Editor->GetSelectionWindows();
	m_ImGuiViewport = &m_Editor->GetGUIViewport();
}

void MaterialEditor::SetupDiffuseSection() {

	SetupSelectorBox(SelectorBoxType::DIFFUSE);
	ImGui::Separator();
}
void MaterialEditor::SetupAmbientSection() {

	SetupSelectorBox(SelectorBoxType::AMBIENT);

	const float DistanceToInputBox = m_LineStartOffset + m_AmbientSectionOffset + m_NameToInputBoxPadding;
	//Strength
	ImGui::SetCursorPos(ImVec2(m_LineStartOffset, ImGui::GetCursorPos().y));
	ImGui::Text("Strength");
	ImGui::SameLine(DistanceToInputBox); //Aligned with the current sprite selector box
	ImGui::SetNextItemWidth(m_CurrentAmbientSelectorBoxSize.x);
	ImGui::InputFloat("##AmbientStrength", &m_Target->m_AmbientStrength);

	ImGui::Separator();
}
void MaterialEditor::SetupSpecularSection() {

	SetupSelectorBox(SelectorBoxType::SPECULAR);

	const float DistanceToInputBox = m_LineStartOffset + m_SpecularSectionOffset + m_NameToInputBoxPadding;
	//Strength
	ImGui::SetCursorPos(ImVec2(m_LineStartOffset, ImGui::GetCursorPos().y));
	ImGui::Text("Strength");
	ImGui::SameLine(DistanceToInputBox);
	ImGui::SetNextItemWidth(m_CurrentSpecularSelectorBoxSize.x);
	ImGui::InputFloat("##SpecularStrength", &m_Target->m_SpecularStrength);

	//Shininess
	ImGui::SetCursorPos(ImVec2(m_LineStartOffset, ImGui::GetCursorPos().y));
	ImGui::Text("Shininess");
	ImGui::SameLine(DistanceToInputBox);
	ImGui::SetNextItemWidth(m_CurrentSpecularSelectorBoxSize.x);
	ImGui::InputInt("##SpecularShininess", &m_Target->m_SpecularShininess);

	ImGui::Separator();
}
	 
void MaterialEditor::SetupSelectorBox(SelectorBoxType type) {

	const float LineStartOffsetX = m_CurrentWindowSize.x * m_LineStartOffsetMultiplier;
	const float SelectorBoxWidth = m_CurrentWindowSize.x * m_SelectorBoxWidthMultiplier;
	const float NameToSelectorBoxPadding = m_CurrentWindowSize.x * m_NameToInputBoxPaddingMultiplier;

	ImVec2 TextureNameTextSize;
	std::string TextureName;

	float DistanceToSelectorBox = 0.0f;
	const float CurrentLineHeight = ImGui::GetCursorPos().y;
	ImGui::SetCursorPos(ImVec2(LineStartOffsetX, CurrentLineHeight)); //Offset from line start

	switch (type) {
	case MaterialEditor::SelectorBoxType::DIFFUSE: {

		//Texture Type/Section Name
		DistanceToSelectorBox = LineStartOffsetX + m_DiffuseSectionOffset + NameToSelectorBoxPadding;
		ImGui::Text("Diffuse");

		ImGui::SameLine(DistanceToSelectorBox);
		ImGui::SetCursorPosY(CurrentLineHeight - 2); //It needs to be offset by -2 to align properly for some reason..

		//Texture Name
		if (m_Target->m_Diffuse != nullptr) {
			TextureName = m_Target->m_Diffuse->GetNameWithoutExtension();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = m_NoneTextSize;
		}

		//Selector Box
		m_CurrentDiffuseSelectorBoxSize = ImVec2(TextureNameTextSize.x + SelectorBoxWidth, m_SelectorBoxHeight);
		if (ImGui::Button("##DiffuseSelector", m_CurrentDiffuseSelectorBoxSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Diffuse);
		}
		ImGui::SameLine(DistanceToSelectorBox + (m_CurrentDiffuseSelectorBoxSize.x / 2) - (TextureNameTextSize.x / 2));

	} break;
	case MaterialEditor::SelectorBoxType::AMBIENT: {

		//Texture Type/Section Name
		DistanceToSelectorBox = LineStartOffsetX + m_AmbientSectionOffset + NameToSelectorBoxPadding;
		ImGui::Text("Ambient");

		ImGui::SameLine(DistanceToSelectorBox);
		ImGui::SetCursorPosY(CurrentLineHeight - 2); //It needs to be offset by -2 to align properly for some reason..

		//Texture Name
		if (m_Target->m_Ambient != nullptr) {
			TextureName = m_Target->m_Ambient->GetNameWithoutExtension();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = m_NoneTextSize;
		}

		//Selector Box
		m_CurrentAmbientSelectorBoxSize = ImVec2(TextureNameTextSize.x + SelectorBoxWidth, m_SelectorBoxHeight);
		if (ImGui::Button("##AmbientSelector", m_CurrentAmbientSelectorBoxSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Ambient);
		}
		ImGui::SameLine(DistanceToSelectorBox + (m_CurrentAmbientSelectorBoxSize.x / 2) - (TextureNameTextSize.x / 2));

	} break;
	case MaterialEditor::SelectorBoxType::SPECULAR: {

		//Texture Type/Section Name
		DistanceToSelectorBox = LineStartOffsetX + m_SpecularSectionOffset + NameToSelectorBoxPadding;
		ImGui::Text("Specular");

		ImGui::SameLine(DistanceToSelectorBox);
		ImGui::SetCursorPosY(CurrentLineHeight - 2); //It needs to be offset by -2 to align properly for some reason..

		//Texture Name
		if (m_Target->m_Specular != nullptr) {
			TextureName = m_Target->m_Specular->GetNameWithoutExtension();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = m_NoneTextSize;
		}

		//Selector Box
		m_CurrentSpecularSelectorBoxSize = ImVec2(TextureNameTextSize.x + SelectorBoxWidth, m_SelectorBoxHeight);
		if (ImGui::Button("##SpecularSelector", m_CurrentSpecularSelectorBoxSize)) {
			m_SelectionWindows->SetSpriteSelectorWindowState(true);
			m_SelectionWindows->SetSpriteSelectorTarget(m_Target->m_Specular);
		}
		ImGui::SameLine(DistanceToSelectorBox + (m_CurrentSpecularSelectorBoxSize.x / 2) - (TextureNameTextSize.x / 2));

	} break;
	default:
		return;
	}

	//Value Name
	ImGui::SetCursorPosY(CurrentLineHeight - 2);
	ImGui::Text(TextureName.data());
}

void MaterialEditor::ApplyStyle() {

	//TODO: Separate style options for each editor section. In this case, the material editor.
	//Style for texture selectors
	//Shouldnt depend on editor style at all.
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.2f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.3f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.4f, 0.0f, 0.0f, 1.0f));
}
void MaterialEditor::ClearStyle() {

	ImGui::PopStyleVar();
	ImGui::PopStyleColor(3);
}

void MaterialEditor::NewFrame() noexcept {

	m_LineStartOffset = m_CurrentWindowSize.x * m_LineStartOffsetMultiplier;
	m_NameToInputBoxPadding = m_CurrentWindowSize.x * m_NameToInputBoxPaddingMultiplier;
}
void MaterialEditor::CheckOffsets() {

	if (m_NoneTextSize.x == 0.0f && m_NoneTextSize.y == 0.0f)
		m_NoneTextSize = ImGui::CalcTextSize("None");

	if (m_DiffuseSectionOffset == 0.0f)
		m_DiffuseSectionOffset = ImGui::CalcTextSize("Diffuse").x;

	if (m_AmbientSectionOffset == 0.0f)
		m_AmbientSectionOffset = ImGui::CalcTextSize("Ambient").x;

	if (m_SpecularSectionOffset == 0.0f)
		m_SpecularSectionOffset = ImGui::CalcTextSize("Specular").x;
}