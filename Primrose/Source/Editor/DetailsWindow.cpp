#include "Editor/DetailsWindow.hpp"

#include "Systems/Core.hpp"
#include "Systems/Editor.hpp"


DetailsWindow::DetailsWindow(Core& core, Editor& editor) noexcept 
	:	m_Core(&core), m_Editor(&editor)
{
}



void DetailsWindow::Update() {



}
void DetailsWindow::Render() {

	if (!m_Opened)
		return;

	UpdateTarget();
	CheckViewportSize();
	UpdateDockData();

	//TODO: Add the ability to reorder these one day

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
	//Note: Will also lock position and size - Use any func without the next to free it

	//MainMenubar is the basis.
	//Details menu can easily be created from it.
	//I can maybe create them in an order like that.

	//It would be MainMenuBar->DetailsWindow then either ContentBrowser or HierarchyWindow. Depending which one decides the height of the other.
	//Probably keep a Clockwise pattern to it.

	//Separate it into Init() and UpdateDockData()

	//NOTE: Regarding sizing. Reset for each window, otherwise update on viewport changes.
	//It seems like these two serve the same purpose kinda.
	//LAST NOTE: Currently, It changes size according to viewport changes and resets position too. It seems ok, its just the pos thing a bit weird.
	//-Check out how engines do it for behavior reference. 
	//Either do it by nums like details window is 60% and content is 40%. or do it by actually getting the size of the other window which creates the order problem.
	//It might not be that big of a problem since this is not meant to be used by users
	if (m_ResetWindow) {
		m_ResetWindow = false;
		ImGui::SetNextWindowSize(m_DockSize);
		ImGui::SetNextWindowPos(m_DockPosition);
	}
	if (m_ViewportUpdated) { //Dups but nearly works. Problem is that window size changes are not viewport related so it wont do it.
		m_ViewportUpdated = false;
		ImGui::SetNextWindowSize(m_DockSize);
		ImGui::SetNextWindowPos(m_DockPosition);
	}


	if (ImGui::Begin("Details", &m_Opened, Flags)) {
		m_Editor->CheckForHoveredWindows();

		if (m_Target != nullptr) {

			//TODO: Most of the elements positions in these functions need to be redone. They are hardcoded. 
			RenderInfoDetails();
			RenderTransformDetails();
			RenderSpriteRendererDetails();
			RenderSkeletalMeshDetails();
			RenderDirectionalLightDetails();
			RenderPointLightDetails();
			RenderSpotLightDetails();


			//Always at the bottom
			RenderAddComponentMenu();
		}

		m_CurrentPosition = ImGui::GetWindowPos();
		m_CurrentSize = ImGui::GetWindowSize();
	}


	ImGui::End();
}
void DetailsWindow::Init() {

	m_SelectionWindows = m_Editor->GetSelectionWindows();
	m_HierarchyWindow = m_Editor->GetHierarchyWindow();
	m_MainMenuBar = m_Editor->GetMainMenuBar();
	m_ImGuiViewport = m_Editor->GetGUIViewport();
}

void DetailsWindow::RenderInfoDetails() {

	ImGui::Text("Name");
	ImGui::SameLine(50.0f);
	ImGui::SetNextItemWidth(100.0f);
	if (ImGui::InputText("##ObjectName", m_NameInputBuffer, m_NameInputBufferSize - 1, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
		m_Target->SetName(m_NameInputBuffer);
	}

	ImGui::SameLine(175.0f);
	bool Enabled = m_Target->GetEnabled();
	if (ImGui::Checkbox("Enabled", &Enabled)) {
		m_Target->SetEnabled(Enabled);
	}

	ImGui::SameLine(300.0f);
	bool Static = m_Target->GetStatic();
	if (ImGui::Checkbox("Static", &Static)) {
		m_Target->SetStatic(Static);
	}

	ImGui::Text("Tag");
	ImGui::SameLine(50.0f);
	ImGui::SetNextItemWidth(100.0f);
	if (ImGui::InputText("##ObjectTag", m_TagInputBuffer, m_TagInputBufferSize - 1, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
		m_Target->SetTag(m_TagInputBuffer);
	}
}
void DetailsWindow::RenderTransformDetails() {

	Transform* SelectedTransform = &m_Target->GetTransform();
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

		ImGui::Text("Position");

		ImGui::SameLine(100.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Text("X");
		ImGui::PopStyleColor();

		ImGui::SameLine(120.0f);
		ImGui::PushItemWidth(50.0F);
		ImGui::InputFloat("##PX", &SelectedTransform->m_Position.m_X);

		ImGui::SameLine(200.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::Text("Y");
		ImGui::PopStyleColor();

		ImGui::SameLine(220.0f);
		ImGui::PushItemWidth(50.0F);
		ImGui::InputFloat("##PY", &SelectedTransform->m_Position.m_Y);

		ImGui::SameLine(300.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		ImGui::Text("Z");
		ImGui::PopStyleColor();

		ImGui::SameLine(320.0f);
		ImGui::PushItemWidth(50.0F);
		ImGui::InputFloat("##PZ", &SelectedTransform->m_Position.m_Z);


		ImGui::Text("Rotation");

		ImGui::SameLine(100.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Text("X");
		ImGui::PopStyleColor();

		ImGui::SameLine(120.0f);
		ImGui::PushItemWidth(50.0F);
		ImGui::InputFloat("##RX", &SelectedTransform->m_Rotation.m_X);

		ImGui::SameLine(200.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::Text("Y");
		ImGui::PopStyleColor();

		ImGui::SameLine(220.0f);
		ImGui::PushItemWidth(50.0F);
		ImGui::InputFloat("##RY", &SelectedTransform->m_Rotation.m_Y);

		ImGui::SameLine(300.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		ImGui::Text("Z");
		ImGui::PopStyleColor();

		ImGui::SameLine(320.0f);
		ImGui::PushItemWidth(50.0F);
		ImGui::InputFloat("##RZ", &SelectedTransform->m_Rotation.m_Z);


		ImGui::Text("Scale");

		ImGui::SameLine(100.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::Text("X");
		ImGui::PopStyleColor();

		ImGui::SameLine(120.0f);
		ImGui::PushItemWidth(50.0F);
		ImGui::InputFloat("##SX", &SelectedTransform->m_Scale.m_X);

		ImGui::SameLine(200.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::Text("Y");
		ImGui::PopStyleColor();

		ImGui::SameLine(220.0f);
		ImGui::PushItemWidth(50.0F);
		ImGui::InputFloat("##SY", &SelectedTransform->m_Scale.m_Y);

		ImGui::SameLine(300.0f);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
		ImGui::Text("Z");
		ImGui::PopStyleColor();

		ImGui::SameLine(320.0f);
		ImGui::PushItemWidth(50.0F);
		ImGui::InputFloat("##SZ", &SelectedTransform->m_Scale.m_Z);

	}
}
void DetailsWindow::RenderSpriteRendererDetails() {

	if (m_Target->HasComponent<SpriteRenderer>()) {
		SpriteRenderer* SelectedSpriteRenderer = m_Target->GetComponent<SpriteRenderer>();
		if (ImGui::CollapsingHeader("Sprite Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {

			//SPRITE
			//IMPORTANT NOTE: For code like this, i used the same color from the style but in case of colors that are not 1.0.0, 0.1.0 and 0.0.1 -
			//- the multiplication will not have the same results. So, in the long term, these values should be member variables and specific in the -
			//- editor style class.
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.2f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.3f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(m_Editor->GetEditorStyle().m_MainColor.m_R * 0.4f, 0.0f, 0.0f, 1.0f));


			ImVec2 ElementTextSize;

			//NOTE: There is a lot of repeated code in the selectors. Consider making it a reusable function!

			//Selectors
			std::string ValueName;
			ImVec2 ValueNameSize;
			ImVec2 InputBoxSize;
			ImVec2 ValueNameTextPosition;

			//Sprite
			//Parameter Name
			ImGui::Text("Sprite");
			ElementTextSize = ImGui::CalcTextSize("Sprite");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));

			//Value Text Calculation
			const Texture2D* Sprite = SelectedSpriteRenderer->GetSprite();
			if (Sprite != nullptr)
				ValueName = Sprite->GetName();
			else
				ValueName = "None";
			ValueNameSize = ImGui::CalcTextSize(ValueName.data());

			//Input Box
			InputBoxSize = ImVec2(m_DockSize.x * 0.2f + ValueNameSize.x, 15.0f);
			if (ImGui::Button("##SpriteNameButton", InputBoxSize)) {
				m_SelectionWindows->SetSpriteSelectorWindowState(true);
				m_SelectionWindows->SetSpriteSelectorTarget(SelectedSpriteRenderer->GetSpriteRef());
			}

			//Value Name Text
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ValueNameTextPosition.x = (ElementTextSize.x + (m_DockSize.x * 0.05f) + InputBoxSize.x / 2) - (ValueNameSize.x / 2);
			ValueNameTextPosition.y = ((ImGui::GetCursorPosY() - InputBoxSize.y / 2) + ValueNameSize.y / 2) - 2; //Offset by -2 to make it look better!
			ImGui::SetCursorPos(ValueNameTextPosition);
			ImGui::Text(ValueName.data());


			//Material
			//Parameter Name
			ImGui::Text("Material");
			ElementTextSize = ImGui::CalcTextSize("Material");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));

			//Value Text Calculation
			const Material* Material = SelectedSpriteRenderer->GetMaterial();
			if (Material != nullptr)
				ValueName = Material->GetAsset().m_Name; //Add GetName() for quicker access and API consistency!
			else
				ValueName = "None";
			ValueNameSize = ImGui::CalcTextSize(ValueName.data());

			//Input Box
			InputBoxSize = ImVec2(m_DockSize.x * 0.2f + ValueNameSize.x, 15.0f);
			if (ImGui::Button("##MaterialNameButton", InputBoxSize)) {
				m_SelectionWindows->SetMaterialSelectorWindowState(true);
				m_SelectionWindows->SetMaterialSelectorTarget(SelectedSpriteRenderer->GetMaterialRef());
			}

			//Value Name Text
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ValueNameTextPosition.x = (ElementTextSize.x + (m_DockSize.x * 0.05f) + InputBoxSize.x / 2) - (ValueNameSize.x / 2);
			ValueNameTextPosition.y = ((ImGui::GetCursorPosY() - InputBoxSize.y / 2) + ValueNameSize.y / 2) - 2; //Offset by -2 to make it look better!
			ImGui::SetCursorPos(ValueNameTextPosition);
			ImGui::Text(ValueName.data());










			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar();

			//Add Material Selector


			m_Editor->AddSpacings(2);



			//TINT
			Color CurrentColor = SelectedSpriteRenderer->GetTint();
			if (ImGui::ColorEdit4("Tint", &CurrentColor.m_R, ImGuiColorEditFlags_NoInputs)) {
				SelectedSpriteRenderer->SetTint(CurrentColor);
			}

			m_Editor->AddSpacings(2);

			//NOTE: this is getting weird with the sets and gets - Might be fine for the sake of using the setters to track changes to values
			//FLIP
			bool FlipX = SelectedSpriteRenderer->GetFlipX();
			bool FlipY = SelectedSpriteRenderer->GetFlipY();
			if (ImGui::Checkbox("FlipX", &FlipX)) {
				SelectedSpriteRenderer->SetFlipX(FlipX);
			}
			ImGui::SameLine(100);
			if (ImGui::Checkbox("FlipY", &FlipY)) {
				SelectedSpriteRenderer->SetFlipY(FlipY);
			}

			m_Editor->AddSpacings(2);


			//BLEND EQUATION
			ImGui::PushItemWidth(200.0f);
			if (ImGui::BeginCombo("Blend Equation", EnumToText::ToText(SelectedSpriteRenderer->GetBlendEquation()))) {

				if (ImGui::Selectable("Additive", false)) {
					SelectedSpriteRenderer->SetBlendEquation(BlendEquation::ADDITIVE);
				}
				if (ImGui::Selectable("Subtractive", false)) {
					SelectedSpriteRenderer->SetBlendEquation(BlendEquation::SUBTRACTIVE);
				}
				if (ImGui::Selectable("Reverse Subtractive", false)) {
					SelectedSpriteRenderer->SetBlendEquation(BlendEquation::REVERSE_SUBTRACTIVE);
				}

				ImGui::EndCombo();
			}

			//SOURCE BLEND MODE
			ImGui::PushItemWidth(200.0f);
			if (ImGui::BeginCombo("Source Blend Mode", EnumToText::ToText(SelectedSpriteRenderer->GetSourceBlendMode()))) {

				if (AddBlendModeSelectable(SourceBlendMode::ZERO)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ZERO);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::ONE)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::SOURCE_COLOR)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::SOURCE_COLOR);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_SOURCE_COLOR)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_SOURCE_COLOR);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::DESTINATION_COLOR)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::DESTINATION_COLOR);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_DESTINATION_COLOR)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_DESTINATION_COLOR);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::SOURCE_ALPHA)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::SOURCE_ALPHA);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_SOURCE_ALPHA)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_SOURCE_ALPHA);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::DESTINATION_ALPHA)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::DESTINATION_ALPHA);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_DESTINATION_ALPHA)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_DESTINATION_ALPHA);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::CONSTANT_COLOR)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::CONSTANT_COLOR);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_CONSTANT_COLOR)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_CONSTANT_COLOR);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::CONSTANT_ALPHA)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::CONSTANT_ALPHA);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_CONSTANT_ALPHA)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_CONSTANT_ALPHA);
				}
				else if (AddBlendModeSelectable(SourceBlendMode::SOURCE_ALPHA_SATURATE)) {
					SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::SOURCE_ALPHA_SATURATE);
				}

				ImGui::EndCombo();
			}

			//DESTINATION BLEND MODE
			ImGui::PushItemWidth(200.0f);
			if (ImGui::BeginCombo("Destination Blend Mode", EnumToText::ToText(SelectedSpriteRenderer->GetDestinationBlendMode()))) {

				if (AddBlendModeSelectable(DestinationBlendMode::ZERO)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ZERO);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::ONE)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::SOURCE_COLOR)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::SOURCE_COLOR);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_SOURCE_COLOR)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_SOURCE_COLOR);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::DESTINATION_COLOR)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::DESTINATION_COLOR);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_DESTINATION_COLOR)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_DESTINATION_COLOR);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::SOURCE_ALPHA)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::SOURCE_ALPHA);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_SOURCE_ALPHA)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_SOURCE_ALPHA);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::DESTINATION_ALPHA)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::DESTINATION_ALPHA);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_DESTINATION_ALPHA)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_DESTINATION_ALPHA);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::CONSTANT_COLOR)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::CONSTANT_COLOR);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_CONSTANT_COLOR)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_CONSTANT_COLOR);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::CONSTANT_ALPHA)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::CONSTANT_ALPHA);
				}
				else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_CONSTANT_ALPHA)) {
					SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_CONSTANT_ALPHA);
				}

				ImGui::EndCombo();
			}


			m_Editor->AddSpacings(2);

			//ADDRESSING MODE
			ImGui::PushItemWidth(100.0f);
			if (ImGui::BeginCombo("Addressing Mode S", EnumToText::ToText(SelectedSpriteRenderer->GetAddressingModeS()))) {

				if (AddAddressingModeSelectable(AddressingMode::CLAMPED_TO_BORDER)) {
					SelectedSpriteRenderer->SetAddressingModeS(AddressingMode::CLAMPED_TO_BORDER);
				}
				else if (AddAddressingModeSelectable(AddressingMode::CLAMPED_TO_EDGE)) {
					SelectedSpriteRenderer->SetAddressingModeS(AddressingMode::CLAMPED_TO_EDGE);
				}
				else if (AddAddressingModeSelectable(AddressingMode::MIRRORED)) {
					SelectedSpriteRenderer->SetAddressingModeS(AddressingMode::MIRRORED);
				}
				else if (AddAddressingModeSelectable(AddressingMode::REPEAT)) {
					SelectedSpriteRenderer->SetAddressingModeS(AddressingMode::REPEAT);
				}

				ImGui::EndCombo();
			}
			ImGui::PushItemWidth(100.0f);
			if (ImGui::BeginCombo("Addressing Mode T", EnumToText::ToText(SelectedSpriteRenderer->GetAddressingModeT()))) {

				if (AddAddressingModeSelectable(AddressingMode::CLAMPED_TO_BORDER)) {
					SelectedSpriteRenderer->SetAddressingModeT(AddressingMode::CLAMPED_TO_BORDER);
				}
				else if (AddAddressingModeSelectable(AddressingMode::CLAMPED_TO_EDGE)) {
					SelectedSpriteRenderer->SetAddressingModeT(AddressingMode::CLAMPED_TO_EDGE);
				}
				else if (AddAddressingModeSelectable(AddressingMode::MIRRORED)) {
					SelectedSpriteRenderer->SetAddressingModeT(AddressingMode::MIRRORED);
				}
				else if (AddAddressingModeSelectable(AddressingMode::REPEAT)) {
					SelectedSpriteRenderer->SetAddressingModeT(AddressingMode::REPEAT);
				}

				ImGui::EndCombo();
			}


			m_Editor->AddSpacings(2);

			//FILTERING MODE
			ImGui::PushItemWidth(170.0f);
			if (ImGui::BeginCombo("Filtering Mode Min", EnumToText::ToText(SelectedSpriteRenderer->GetFilteringModeMin()))) {

				if (AddFilteringModeMinSelectable(FilteringModeMin::LINEAR)) {
					SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::LINEAR);
				}
				else if (AddFilteringModeMinSelectable(FilteringModeMin::LINEAR_MIPMAP_LINEAR)) {
					SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::LINEAR_MIPMAP_LINEAR);
				}
				else if (AddFilteringModeMinSelectable(FilteringModeMin::LINEAR_MIPMAP_NEAREST)) {
					SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::LINEAR_MIPMAP_NEAREST);
				}
				else if (AddFilteringModeMinSelectable(FilteringModeMin::NEAREST)) {
					SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::NEAREST);
				}
				else if (AddFilteringModeMinSelectable(FilteringModeMin::NEAREST_MIPMAP_LINEAR)) {
					SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::NEAREST_MIPMAP_LINEAR);
				}
				else if (AddFilteringModeMinSelectable(FilteringModeMin::NEAREST_MIPMAP_NEAREST)) {
					SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::NEAREST_MIPMAP_NEAREST);
				}

				ImGui::EndCombo();
			}
			ImGui::PushItemWidth(170.0f);
			if (ImGui::BeginCombo("Filtering Mode Mag", EnumToText::ToText(SelectedSpriteRenderer->GetFilteringModeMag()))) {

				if (AddFilteringModeMagSelectable(FilteringModeMag::LINEAR)) {
					SelectedSpriteRenderer->SetFilteringModeMag(FilteringModeMag::LINEAR);
				}
				else if (AddFilteringModeMagSelectable(FilteringModeMag::NEAREST)) {
					SelectedSpriteRenderer->SetFilteringModeMag(FilteringModeMag::NEAREST);
				}

				ImGui::EndCombo();
			}


		}
	}
}
void DetailsWindow::RenderSkeletalMeshDetails() {

	if (m_Target->HasComponent<SkeletalMesh>()) {
		//SkeletalMesh* SelectedSkeletalMesh = m_SelectedGameObject->GetComponent<SkeletalMesh>();
		if (ImGui::CollapsingHeader("Skeletal Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {


		}
	}
}
void DetailsWindow::RenderDirectionalLightDetails() {

	if (m_Target->HasComponent<DirectionalLight>()) {
		DirectionalLight* SelectedDirectionalLight = m_Target->GetComponent<DirectionalLight>();
		if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_DefaultOpen)) {

			ImVec2 ElementTextSize;

			//Tint
			ImGui::Text("Tint");
			ElementTextSize = ImGui::CalcTextSize("Tint");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			Color CurrentColor = SelectedDirectionalLight->m_Tint;
			if (ImGui::ColorEdit4("##TintPicker", &CurrentColor.m_R, ImGuiColorEditFlags_NoInputs)) {
				SelectedDirectionalLight->m_Tint = CurrentColor;
			}

			//Intensity
			ImGui::Text("Intensity");
			ElementTextSize = ImGui::CalcTextSize("Intensity");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ImGui::SetNextItemWidth(m_DockSize.x * 0.4f);
			ImGui::SliderFloat("##Intensity", &SelectedDirectionalLight->m_Intensity, 0.0f, 10.0f);

		}
	}
}
void DetailsWindow::RenderPointLightDetails() {

	if (m_Target->HasComponent<PointLight>()) {
		PointLight* SelectedPointLight = m_Target->GetComponent<PointLight>();
		if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen)) {

			ImVec2 ElementTextSize;

			//TODO: Test this after making the details window size dynamic.

			//Tint
			ImGui::Text("Tint");
			ElementTextSize = ImGui::CalcTextSize("Tint");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			Color CurrentColor = SelectedPointLight->m_Tint;
			if (ImGui::ColorEdit4("##TintPicker", &CurrentColor.m_R, ImGuiColorEditFlags_NoInputs)) {
				SelectedPointLight->m_Tint = CurrentColor;
			}

			//Intensity
			ImGui::Text("Intensity");
			ElementTextSize = ImGui::CalcTextSize("Intensity");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ImGui::SetNextItemWidth(m_DockSize.x * 0.4f);
			ImGui::SliderFloat("##Intensity", &SelectedPointLight->m_Intensity, 0.0f, 10.0f);

			//Attenuation
			ImGui::Text("Attenuation");
			ElementTextSize = ImGui::CalcTextSize("Attenuation");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ImGui::SetNextItemWidth(m_DockSize.x * 0.4f);
			ImGui::SliderFloat("##Attenuation", &SelectedPointLight->m_Attenuation, 0.0f, 10.0f);

			//SourceRadius
			ImGui::Text("Source Radius");
			ElementTextSize = ImGui::CalcTextSize("Source Radius");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ImGui::SetNextItemWidth(m_DockSize.x * 0.4f);
			ImGui::SliderFloat("##Source Radius", &SelectedPointLight->m_SourceRadius, 0.0f, 10.0f);
		}
	}
}
void DetailsWindow::RenderSpotLightDetails() {

	if (m_Target->HasComponent<SpotLight>()) {
		SpotLight* SelectedSpotLight = m_Target->GetComponent<SpotLight>();
		if (ImGui::CollapsingHeader("Spot Light", ImGuiTreeNodeFlags_DefaultOpen)) {

			ImVec2 ElementTextSize;

			//Tint
			ImGui::Text("Tint");
			ElementTextSize = ImGui::CalcTextSize("Tint");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			Color CurrentColor = SelectedSpotLight->m_Tint;
			if (ImGui::ColorEdit4("##TintPicker", &CurrentColor.m_R, ImGuiColorEditFlags_NoInputs)) {
				SelectedSpotLight->m_Tint = CurrentColor;
			}

			//Intensity
			ImGui::Text("Intensity");
			ElementTextSize = ImGui::CalcTextSize("Intensity");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ImGui::SetNextItemWidth(m_DockSize.x * 0.4f);
			ImGui::SliderFloat("##Intensity", &SelectedSpotLight->m_Intensity, 0.0f, 10.0f);

			//Inner Cutoff Angle
			ImGui::Text("Inner Cutoff Angle");
			ElementTextSize = ImGui::CalcTextSize("Inner Cutoff Angle");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ImGui::SetNextItemWidth(m_DockSize.x * 0.4f);
			ImGui::SliderFloat("##Inner Cutoff Angle", &SelectedSpotLight->m_InnerCutoffAngle, 1.0f, 180.0f);

			//Outer Cutoff Angle
			ImGui::Text("Outer Cutoff Angle");
			ElementTextSize = ImGui::CalcTextSize("Outer Cutoff Angle");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ImGui::SetNextItemWidth(m_DockSize.x * 0.4f);
			ImGui::SliderFloat("##Outer Cutoff Angle", &SelectedSpotLight->m_OuterCutoffAngle, 1.0f, 180.0f);

			//Attenuation
			ImGui::Text("Attenuation");
			ElementTextSize = ImGui::CalcTextSize("Attenuation");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ImGui::SetNextItemWidth(m_DockSize.x * 0.4f);
			ImGui::SliderFloat("##Attenuation", &SelectedSpotLight->m_Attenuation, 0.0f, 10.0f);

			//SourceRadius
			ImGui::Text("Source Radius");
			ElementTextSize = ImGui::CalcTextSize("Source Radius");
			ImGui::SameLine(ElementTextSize.x + (m_DockSize.x * 0.05f));
			ImGui::SetNextItemWidth(m_DockSize.x * 0.4f);
			ImGui::SliderFloat("##Source Radius", &SelectedSpotLight->m_SourceRadius, 0.0f, 10.0f);

		}
	}
}
void DetailsWindow::RenderAddComponentMenu() {

	ImGui::Separator();
	m_Editor->AddSpacings(6);
	ImGui::SameLine((m_DockSize.x / 2) - 75.0f);
	if (ImGui::Button("Add Component", ImVec2(150.0f, 20.0f))) {
		ImVec2 PopupPosition = { ImGui::GetMousePos().x - 75, ImGui::GetMousePos().y }; //75 - Half box width
		ImGui::SetNextWindowPos(PopupPosition);
		ImGui::OpenPopup("AddComponentMenu");
	}

	if (ImGui::BeginPopup("AddComponentMenu")) {

		if (ImGui::Selectable("SpriteRenderer", false, 0, ImVec2(150, 20))) {
			m_Target->AddComponent<SpriteRenderer>();
		}
		else if (ImGui::Selectable("SkeletalMesh", false, 0, ImVec2(150, 20))) {
			m_Target->AddComponent<SkeletalMesh>();
		}
		else if (ImGui::Selectable("Camera", false, 0, ImVec2(150, 20))) {
			m_Target->AddComponent<Camera>();
		}
		else if (ImGui::Selectable("DirectionalLight", false, 0, ImVec2(150, 20))) {
			m_Target->AddComponent<DirectionalLight>();
		}
		else if (ImGui::Selectable("PointLight", false, 0, ImVec2(150, 20))) {
			m_Target->AddComponent<PointLight>();
		}
		else if (ImGui::Selectable("SpotLight", false, 0, ImVec2(150, 20))) {
			m_Target->AddComponent<SpotLight>();
		}

		ImGui::EndPopup();
	}
}

bool DetailsWindow::AddBlendModeSelectable(SourceBlendMode mode) {
	using namespace EnumToText;

	if (ImGui::Selectable(ToText(mode), false))
		return true;
	else
		return false;
}
bool DetailsWindow::AddBlendModeSelectable(DestinationBlendMode mode) {
	using namespace EnumToText;

	if (ImGui::Selectable(ToText(mode), false))
		return true;
	else
		return false;
}
bool DetailsWindow::AddAddressingModeSelectable(AddressingMode mode) {
	using namespace EnumToText;

	if (ImGui::Selectable(ToText(mode), false))
		return true;
	else
		return false;
}
bool DetailsWindow::AddFilteringModeMinSelectable(FilteringModeMin mode) {
	using namespace EnumToText;

	if (ImGui::Selectable(ToText(mode), false))
		return true;
	else
		return false;
}
bool DetailsWindow::AddFilteringModeMagSelectable(FilteringModeMag mode) {
	using namespace EnumToText;

	if (ImGui::Selectable(ToText(mode), false))
		return true;
	else
		return false;
}

void DetailsWindow::UpdateTarget() noexcept {

	GameObject* CurrentTarget = m_HierarchyWindow->GetSelectedGameObject();
	if (CurrentTarget == nullptr) {
		m_Target = nullptr;
		return;
	}
	else if (CurrentTarget != m_Target) {
		m_Target = CurrentTarget;
		strcpy_s(m_NameInputBuffer, m_Target->GetName().c_str());
		strcpy_s(m_TagInputBuffer, m_Target->GetTag().c_str());
	}


	//This here is terrible and needs to be fully reworked!
	//wot
	//THIS WHOLE THING NEEDS TO BE REWORKED I THINK!
	if (m_SelectionWindows->GetSpriteSelectorTarget() != nullptr) ////////????????? Material then...
		m_SelectionWindows->ClearSpriteSelectorTarget();
}
void DetailsWindow::UpdateDockData() noexcept {
	//Check viewport changes only. If so then change dock positions? no reset?
	// Maybe do another reset after init is finished or something cause i need all windows to have their positions calculated before i adjust!
	//I think they need to be kept updated each frame to adjust to me resizing the window! i do need update positions function after all!
	//m_DockSize = ImVec2(m_ImGuiViewport->Size.x * 0.2f, m_ImGuiViewport->Size.y - m_MainMenuBar->GetSize().y - m_ContentBrowser->GetContentBrowserWindowDockSize().y); //? wot also no content browser
	//m_DockPosition = ImVec2(m_ImGuiViewport->Size.x - m_DockSize.x, m_MainMenuBar->GetSize().y);
	m_DockSize = ImVec2(m_ImGuiViewport->Size.x * 0.2f, m_ImGuiViewport->Size.y - m_MainMenuBar->GetSize().y); //? wot also no content browser
	m_DockPosition = ImVec2(m_ImGuiViewport->Size.x - m_DockSize.x, m_MainMenuBar->GetSize().y);
}
void DetailsWindow::CheckViewportSize() noexcept {

	if (m_ImGuiViewport->Size.x != m_LastViewportSize.x || m_ImGuiViewport->Size.y != m_LastViewportSize.y) {
		m_ViewportUpdated = true;
		m_LastViewportSize = m_ImGuiViewport->Size;
	}
}