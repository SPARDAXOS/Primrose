#include "Editor.hpp"
#include "Core.hpp"



#include <iostream>


Editor::Editor(Core& core)
	: m_EngineCore(&core)
{
	m_WindowReference = m_EngineCore->GetWindow();
	m_ECSReference = m_EngineCore->GetECS();
	m_TextureStorageReference = m_EngineCore->GetTextureStorage();
	m_AssetManagerReference = m_EngineCore->GetAssetManager();
	m_InputReference = m_EngineCore->GetInput();
	m_TimeReference = m_EngineCore->GetTime();

	//Get folder texture?

	IMGUI_CHECKVERSION();
	m_GUIContext = ImGui::CreateContext();
	m_GUIViewport = ImGui::GetMainViewport();


	//CalculateWindowsSizes()

	m_Logger.SetTimeReference(*m_TimeReference); //Since it will have nullptr on this class' construction
	SystemLog(ImGui::GetVersion());

	m_ViewportCameraReference = &m_ECSReference->GetViewportCamera();
	if (m_ViewportCameraReference == nullptr)
		SystemLog("Failed to save viewport camera reference");

	m_IO = &ImGui::GetIO();
	m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// IF using Docking Branch
	m_IO->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; //Disable IMGUI from interfering with glfw when it comes to cursor visibility and shape



	ImGui_ImplGlfw_InitForOpenGL(m_WindowReference->GetWindowResource().m_Handle, true);
	ImGui_ImplOpenGL3_Init();

	//logger!
}
Editor::~Editor() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


[[nodiscard]] bool Editor::Update() {

	Render();
	CheckInput();
	UpdateViewportControls();

	return true;
}


void Editor::SaveEngineTexturesReferences() {

	if (!m_TextureStorageReference->GetTexture2D("Folder", m_FolderTexture))
		SystemLog("Failed to save reference to engine texture [Folder]");

	if (!m_TextureStorageReference->GetTexture2D("Debug", m_DebugTexture))
		SystemLog("Failed to save reference to engine texture [Debug]");

	if (!m_TextureStorageReference->GetTexture2D("Warning", m_WarningTexture))
		SystemLog("Failed to save reference to engine texture [Warning]");

	if (!m_TextureStorageReference->GetTexture2D("Error", m_ErrorTexture))
		SystemLog("Failed to save reference to engine texture [Error]");

}
void Editor::DebugLog(std::string message) noexcept {
	m_Logger.DebugLog(message);
}
void Editor::WarningLog(std::string message) noexcept {
	m_Logger.WarningLog(message);
}
void Editor::ErrorLog(std::string message) noexcept {
	m_Logger.ErrorLog(message);
}
void Editor::SystemLog(std::string message) noexcept {
	m_Logger.SystemLog(message);
}

void Editor::Render() {//TODO: REturn const to this after getting rid of the text transform

	StartFrame();
	m_IsAnyWindowHovered = false;
	m_Logger.NewFrame();

	SetupEditorStyle();


 //Important that nothing calls this but this window to put it behind everything
	//ViewportFlags |= ImGuiWindowFlags_NoCollapse;
	//ViewportFlags |= ImGuiWindowFlags_NoMove;
	//ViewportFlags |= ImGuiWindowFlags_NoTitleBar;
	//ViewportFlags |= ImGuiWindowFlags_NoDocking;
	
	
	//ImGui::ShowDemoWindow();


	//IMPORTANT NOTE: Make the size of each element in relation to the size of the viewport so they would scale with it
	//TODO: All the menus sizes and positions are relative to each other. Make sure to make the values used relative to each other instead of relying on literals
	RenderDetailsMenu();
	RenderHeirarchyMenu();
	RenderAddGameObjectButton();
	RenderDirectoryExplorer();
	RenderContentWindow();
	RenderViewportWindow();
	RenderStandaloneWindows();

	RenderMainMenuBar();





	//ImGuiWindowFlags Flags = 0;
	//Flags |= ImGuiWindowFlags_NoTitleBar;
	//Flags |= ImGuiWindowFlags_NoCollapse;
	//Flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
	//Flags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
	//Flags |= ImGuiWindowFlags_NoBackground;
	//Flags |= ImGuiWindowFlags_MenuBar;

	

	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
	//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	//ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	//if (ImGui::Begin("TestBarMenu", nullptr, Flags)) {
	//	//ImGui::SetWindowPos(m_ContentBrowserWindowPosition);

	//	ImGui::PopStyleColor();
	//	ImGui::PopStyleColor();
	//	ImGui::PopStyleColor();

	//	ImGui::SetCursorPos(ImVec2(0, 0));
	//	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
	//	if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable)) {

	//		if (ImGui::BeginTabItem("ContentBrowserTest")) {

	//			ImGui::EndTabItem();
	//		}
	//		if (ImGui::BeginTabItem("ContentBrowserTest2")) {

	//			ImGui::EndTabItem();
	//		}
	//		if (ImGui::BeginTabItem("ContentBrowserTest3")) {

	//			ImGui::EndTabItem();
	//		}

	//		ImGui::EndTabBar();
	//	}

	//	ImGui::SetCursorPos(ImVec2(0, 20));

	//	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.7f));
	//	ImGui::SetNextWindowSize(ImVec2(m_ContentBrowserWindowSize.x, m_ContentBrowserWindowSize.y));
	//	if (ImGui::BeginChild("Child", ImGui::GetContentRegionMax(), false, 0)) {


	//		ImGui::EndChild();
	//	}

	//	ImGui::PopStyleColor();
	//	ImGui::PopStyleColor();
	//}
	//	
	//ImGui::End();

	//ImGui::ShowMetricsWindow(); Good for debugging

	//if (ImGui::Begin("TestWindow", 0, ImGuiWindowFlags_NoDocking)) {

	//	m_TestPosition = ImGui::GetWindowPos();
	//	ImGui::SetWindowPos(m_TestPosition);

	//}

	//ImGui::End();

	ClearEditorStyle();

	RenderFrame();
}
void Editor::StartFrame() const {
	//Feed input and start GUI frame

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
void Editor::RenderFrame() const {
	//Render GUI to screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void Editor::RenderDetailsMenu() { //TODO: REturn const to this after getting rid of the text transform

	if (!m_DetailsWindowOpened)
		return;

	//Notes:
	//This could be abstracted away since DRY


	//TODO: Add the ability to reorder these one day


	//Render GUI
	//Note: Will also lock position and size - Use any func without the next to free it
	ImGui::SetNextWindowSize(ImVec2(400.0f, m_GUIViewport->Size.y - m_MainMenuBarSize.y));
	ImGui::SetNextWindowPos(ImVec2(m_GUIViewport->Size.x - 400.0f, m_MainMenuBarSize.y));

	ImGui::Begin("Details", &m_DetailsWindowOpened);
	CheckForHoveredWindows();

	if (m_SelectedGameObject != nullptr) {

		//TODO: Abstract these into separate functions for clarity

		////////////
		//Info
		////////////
		ImGui::Text("Name");
		ImGui::SameLine(50.0f);
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::InputText("##ObjectName", m_NameInputBuffer, 32, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
			m_SelectedGameObject->SetName(m_NameInputBuffer);
		}

		ImGui::SameLine(175.0f);
		bool Enabled = m_SelectedGameObject->GetEnabled();
		if (ImGui::Checkbox("Enabled", &Enabled)) {
			m_SelectedGameObject->SetEnabled(Enabled);
		}

		ImGui::SameLine(300.0f);
		bool Static = m_SelectedGameObject->GetStatic();
		if (ImGui::Checkbox("Static", &Static)) {
			m_SelectedGameObject->SetStatic(Static);
		}

		ImGui::Text("Tag");
		ImGui::SameLine(50.0f);
		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::InputText("##ObjectTag", m_TagInputBuffer, 32, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
			m_SelectedGameObject->SetTag(m_TagInputBuffer);
		}



		////////////
		//Transform
		////////////
		Transform* SelectedTransform = &m_SelectedGameObject->GetTransform();
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


		////////////
		//SpriteRenderer
		////////////
		if (m_SelectedGameObject->HasComponent<SpriteRenderer>()) {
			SpriteRenderer* SelectedSpriteRenderer = m_SelectedGameObject->GetComponent<SpriteRenderer>();
			if (ImGui::CollapsingHeader("SpriteRenderer", ImGuiTreeNodeFlags_DefaultOpen)) {

				//TINT
				Color CurrentColor = SelectedSpriteRenderer->GetTint();
				if (ImGui::ColorEdit4("Tint", &CurrentColor.m_R, ImGuiColorEditFlags_NoInputs)) {
					SelectedSpriteRenderer->SetTint(CurrentColor);
				}

				ImGui::Spacing();
				ImGui::Spacing();

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


				ImGui::Spacing();
				ImGui::Spacing();
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


				ImGui::Spacing();
				ImGui::Spacing();
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


				ImGui::Spacing();
				ImGui::Spacing();
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


		////////////
		//AddComponent
		////////////
		//Always at the bottom
		ImGui::Separator();
		AddSpacings(6);
		ImGui::SameLine(150);
		if (ImGui::Button("Add Component", ImVec2(100.0f, 20.0f))) {
			ImVec2 PopupPosition = { ImGui::GetMousePos().x - 75, ImGui::GetMousePos().y }; //75 - Half box width
			ImGui::SetNextWindowPos(PopupPosition);
			ImGui::OpenPopup("AddComponentMenu");
		}

		if (ImGui::BeginPopup("AddComponentMenu")) {

			if (ImGui::Selectable("SpriteRenderer", false, 0, ImVec2(150, 20))) {
				m_SelectedGameObject->AddComponent<SpriteRenderer>();
			}
			else if (ImGui::Selectable("Camera", false, 0, ImVec2(150, 20))) {
				m_SelectedGameObject->AddComponent<Camera>();
			}

			ImGui::EndPopup();
		}
		//Popup with options
	}

	ImGui::End();
}
void Editor::RenderHeirarchyMenu() {

	if (!m_HeirarchyWindowOpened)
		return;

	ImGui::SetNextWindowSize(ImVec2(200.0f, m_GUIViewport->Size.y - 300.0f - m_MainMenuBarSize.y)); // 50.0f Size of Add Menu
	ImGui::SetNextWindowPos(ImVec2(0.0f, m_MainMenuBarSize.y)); // 200 Size of Add Menu
	//ImGuiWindowFlags_NoBringToFrontOnFocus
	ImGui::Begin("Heirarchy", &m_HeirarchyWindowOpened);

	CheckForHoveredWindows();

	//TODO: This needs cleanup, especially the id check down there. Like test if the ID is reserved or something
	std::vector<GameObject*> GameObjects = m_ECSReference->GetGameObjects();
	for (uint32 i = 0; i < GameObjects.size(); i++) {
		if (GameObjects.at(i)->GetObjectID() != VIEWPORT_CAMERA_OBJECT_ID && GameObjects.at(i)->GetParent() == nullptr)
			AddHeirarchyEntry(GameObjects.at(i));
	}

	ImGui::End();
	
}
void Editor::RenderAddGameObjectButton() {

	if (!m_HeirarchyWindowOpened)
		return;

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoTitleBar;
	Flags |= ImGuiWindowFlags_NoResize;
	Flags |= ImGuiWindowFlags_NoMove;
	Flags |= ImGuiWindowFlags_NoScrollbar;
	Flags |= ImGuiWindowFlags_NoBackground;
	Flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	//Flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	//NOTE: put this at the edge of the hierarchy bar!
	ImGui::SetNextWindowSize(ImVec2(100.0f, 0.0f)); //As big as the button
	ImGui::SetNextWindowPos(ImVec2(122.0f, -7.5f + m_MainMenuBarSize.y)); //BUG: Even after all this hardcoding this is still slightly off. It is visible
	ImGui::Begin("##AddGameObjectMenu", &m_HeirarchyWindowOpened, Flags);
	CheckForHoveredWindows();

	if (ImGui::Button("##AddButton", ImVec2(70.0f, 19.0f))) {
		const ImVec2 PopupPosition = { ImGui::GetMousePos().x - 75, ImGui::GetMousePos().y }; //75 - Half box width
		ImGui::SetNextWindowPos(PopupPosition);
		ImGui::OpenPopup("AddGameObjectMenu");
	}

	ImGui::SameLine(30.0f, 1.0f); //Manually adjusted
	ImGui::Text("Add..");

	if (ImGui::BeginPopup("AddGameObjectMenu")) {

		if (ImGui::Selectable("Empty GameObject", false, 0, ImVec2(150, 20))) {
			m_ECSReference->CreateGameObject();
		}
		else if (ImGui::Selectable("Cube", false, 0, ImVec2(150, 20))) {
			GameObject* Object = &m_ECSReference->CreateGameObject("Cube");
			Object->AddComponent<SpriteRenderer>(); //CHANGE THIS WHEN I HAVE ACTUAL 3D MESHES
		}
		else if (ImGui::Selectable("Sprite", false, 0, ImVec2(150, 20))) {
			GameObject* Object = &m_ECSReference->CreateGameObject("Sprite"); //This breaks for some reason
			Object->AddComponent<SpriteRenderer>(); //CHANGE THIS WHEN I HAVE ACTUAL 3D MESHES
		}
		else if (ImGui::Selectable("Camera", false, 0, ImVec2(150, 20))) {
			GameObject* Object = &m_ECSReference->CreateGameObject("Camera"); //This breaks for some reason
			Object->AddComponent<Camera>(); //CHANGE THIS WHEN I HAVE ACTUAL 3D MESHES
		}


		ImGui::EndPopup();
	}

	ImGui::End();
}
void Editor::RenderDirectoryExplorer() {

	if (!m_DirectoryExplorerWindowOpened)
		return;

	ImGui::SetNextWindowSize(m_DirectoryExplorerWindowSize);
	ImGui::SetNextWindowPos(ImVec2(0.0f, m_GUIViewport->Size.y - m_DirectoryExplorerWindowSize.y));

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

	ImGui::Begin("Directory Explorer", &m_DirectoryExplorerWindowOpened, Flags);
	CheckForHoveredWindows();

	auto Root = m_AssetManagerReference->GetRoot();
	if (m_SelectedDirectory == nullptr)
		m_SelectedDirectory = Root;

	AddFileExplorerEntry(Root);

	ImGui::End();
}
void Editor::RenderContentWindow() {


	//Might be unnecessary to use this member variable but i can guarantee that the window will not appear then.
	m_ContentWindowOpened = false;
	m_ContentWindowOpened |= m_ContentBrowserOpened;
	m_ContentWindowOpened |= m_DebugLogOpened;
	m_ContentWindowOpened |= m_SystemLogOpened;
	if (!m_ContentWindowOpened)
		return;

	//Reset focus
	m_ContentBrowserFocusedInTab = false;
	m_DebugLogFocusedInTab = false;
	m_SystemLogFocusedInTab = false;





	ImGuiWindowFlags TabsWindowFlags = 0;
	TabsWindowFlags |= ImGuiWindowFlags_NoCollapse;
	TabsWindowFlags |= ImGuiWindowFlags_NoTitleBar;
	TabsWindowFlags |= ImGuiWindowFlags_NoScrollbar;
	//TabsWindowFlags |= ImGuiWindowFlags_NoResize;
	TabsWindowFlags |= ImGuiWindowFlags_NoBackground;
	//TabsWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	TabsWindowFlags |= ImGuiWindowFlags_AlwaysAutoResize;


	//Should be cleaned in new size and pos calculation
	m_ContentWindowPosition = ImVec2(m_DirectoryExplorerWindowSize.x, m_GUIViewport->Size.y - m_ContentWindowSize.y);
	m_ContentWindowTabsPosition.x = m_ContentWindowPosition.x;

	std::string TabName = "Error";

	ImGui::SetNextWindowSize(m_ContentWindowTabsSize);
	ImGui::SetNextWindowPos(m_ContentWindowTabsPosition);
	if (ImGui::Begin("##TabsWindow", &m_ContentBrowserOpened, TabsWindowFlags)) {
		CheckForHoveredWindows();

		if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable)) {
			
			if (!m_ContentBrowserOpenedStandalone && ImGui::BeginTabItem("ContentBrowser", &m_ContentBrowserOpened)) {
				m_ContentBrowserFocusedInTab = true;

				//std::cout << "Any Window Hovered! " << ImGui::IsItemHovered() << std::endl;

				//It doesnt work - Get mouse pos as im holding down the button
				// -Check if i simply pass out the tabs and bounds
				//Do it manually
				static ImVec2 NewPos;
				static ImVec2 LastPos;
				static ImVec2 DeltaPos;

				NewPos = ImGui::GetMousePos();
				DeltaPos = ImVec2(NewPos.x - LastPos.x, NewPos.y - LastPos.y);
				LastPos = NewPos;
				//This could work but it depends on where on the tab i click cause the distance to outside of it is different then
				m_ContentWindowTabsPosition.y = m_GUIViewport->Size.y - (m_ContentWindowSize.y + m_ContentWindowTabsSize.y);
				bool CheckResults = IsPointInBoundingBox(ImGui::GetMousePos(), m_ContentWindowTabsPosition, m_ContentWindowTabsSize);
				//std::cout << "X " << DeltaPos.x << " Y " << DeltaPos.y << std::endl;
				std::cout << "Results: " << CheckResults << std::endl;


				//Final note: It works. I did not test it extensively but it worked!. Although getting the y for the pos is a problem
				//This function needs a lot of cleaning and refactoring

				//NOTE: Worst case i do a manual collision check where i send the position and bounding box size of this window and-
				// calculate whether the mouse cursor is in it or not!
				//ImGui::IsAnyItemHovered() Works but the gap between each item counts as nothing hovered so it breaks

				//IsItemHovered() also works but it requires the user to drag even faster
				
				//Conclusion-
				//It seems that the gap between the tabs is the problem here
				//Its either i find a way to deal with that or simply setup a timer for how long the after no item is hovered it triggers a drag
				//This solution is a bit messy since you could hold still in the gap or to the side of the bar for it to break. Manual collision-
				//-solution is more sturdy.

				//Another solution is to check mouse drag delta
				//If the mouse drag delta exceeds the size.y of the tab then im dragging outside of it.
				//-Otherwise im reordering

				if (!CheckResults && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoDisableHover)) {
					const auto Data = &m_ContentBrowserOpenedStandalone;
					ImGui::SetDragDropPayload("Tab", &Data, sizeof(Data));
					ImGui::EndDragDropSource();
				}

				TabName = "Content";
				ImGui::EndTabItem();
			}
			if (!m_DebugLogOpenedStandalone && ImGui::BeginTabItem("DebugLog", &m_DebugLogOpened)) {
				m_DebugLogFocusedInTab = true;

				if (ImGui::BeginDragDropSource()) {
					const auto Data = &m_DebugLogOpenedStandalone;
					ImGui::SetDragDropPayload("Tab", &Data, sizeof(Data));
					ImGui::EndDragDropSource();
				}

				TabName = "Debug";
				ImGui::EndTabItem();
			}
			if (!m_SystemLogOpenedStandalone && ImGui::BeginTabItem("SystemLog", &m_SystemLogOpened)) {
				m_SystemLogFocusedInTab = true;

				if (ImGui::BeginDragDropSource()) {
					const auto Data = &m_SystemLogOpenedStandalone;
					ImGui::SetDragDropPayload("Tab", &Data, sizeof(Data));
					ImGui::EndDragDropSource();
				}

				TabName = "System";
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}


	ImGuiWindowFlags ContentWindowFlags = 0;
	ContentWindowFlags |= ImGuiWindowFlags_NoCollapse;
	ContentWindowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
	ContentWindowFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
	//ContentWindowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
	//ContentWindowFlags |= ImGuiWindowFlags_NoResize;
	if (m_DebugLogFocusedInTab || m_SystemLogFocusedInTab)
		ContentWindowFlags |= ImGuiWindowFlags_MenuBar;

	ImGui::SetNextWindowSize(m_ContentWindowSize);
	ImGui::SetNextWindowPos(m_ContentWindowPosition);
	if (ImGui::Begin(TabName.data(), &m_ContentWindowOpened, ContentWindowFlags)) {
		CheckForHoveredWindows(); //Not Working? Check this

		//Should probably do another check if any window is in focus, otherwise its a crash!

		//std::cout << ImGui::GetWindowContentRegionMax().x << " Y " << ImGui::GetWindowContentRegionMax().y << std::endl;
		//Its one frame behind so it looks like its animated (Lagging behind)
		m_ContentWindowTabsPosition.y = m_GUIViewport->Size.y - (ImGui::GetWindowSize().y + m_ContentWindowTabsSize.y);
		//m_ContentWindowTabsPosition.y = ImGui::GetWindowContentRegionMax().y;



		if (m_ContentBrowserFocusedInTab)
			RenderContentBrowser();
		else if (m_DebugLogFocusedInTab)
			RenderDebugLog();
		else if (m_SystemLogFocusedInTab)
			RenderSystemLog();

		ImGui::End();
	}


	//Check if content window was closed by clicking the X and close all tabs then
	//TODO: Make functions CloseAllContentTabs() to more easily  keep track of new tabs
	if (!m_ContentWindowOpened) {
		m_ContentBrowserOpened = false;
		m_DebugLogOpened = false;
		m_SystemLogOpened = false;
	}
}
void Editor::RenderMainMenuBar() {

	if (ImGui::BeginMainMenuBar()) {

		//Need to query height
		if (m_MainMenuBarSize.x == 0.0f && m_MainMenuBarSize.y == 0.0f)
			m_MainMenuBarSize = ImGui::GetWindowSize();

		//File
		if (ImGui::BeginMenu("File")) {

			ImGui::MenuItem("Test1", "CTRL + D");
			ImGui::MenuItem("Test2", "CTRL + D");
			ImGui::MenuItem("Test3", "CTRL + D");
			ImGui::MenuItem("Test4", "CTRL + D");


			ImGui::EndMenu();
		}

		//Window
		if (ImGui::BeginMenu("Window")) {

			ImGui::MenuItem("Details", "CTRL + D", &m_DetailsWindowOpened);
			ImGui::MenuItem("Heirarchy", "CTRL + D", &m_HeirarchyWindowOpened);
			ImGui::MenuItem("Content Browser", "CTRL + D", &m_ContentBrowserOpened);
			ImGui::MenuItem("Directory Explorer", "CTRL + D", &m_DirectoryExplorerWindowOpened);

			if (ImGui::BeginMenu("Logs")) {

				ImGui::MenuItem("Debug Log", "CTRL + D", &m_DebugLogOpened);
				ImGui::MenuItem("System Log", "CTRL + D", &m_SystemLogOpened);

				ImGui::EndMenu();
			}


			ImGui::EndMenu();
		}


		//About - Should be at the very end always!
		ImGuiWindowFlags AboutMenuFlags = 0;
		if (ImGui::MenuItem("About")) {
			ImGui::OpenPopup("AboutMenu");
			AboutMenuFlags |= ImGuiWindowFlags_NoTitleBar;
			AboutMenuFlags |= ImGuiWindowFlags_NoResize;
			AboutMenuFlags |= ImGuiWindowFlags_NoMove;
			AboutMenuFlags |= ImGuiWindowFlags_NoScrollbar;
		}
		if (ImGui::BeginPopupModal("AboutMenu", nullptr, AboutMenuFlags)) {
			
			ImGui::SetWindowPos(ImVec2((m_GUIViewport->Size.x / 2) - 200.0f, (m_GUIViewport->Size.y / 2) - 100.0f));
			ImGui::SetWindowSize(ImVec2(400.0f, 200.0f));

			AddSpacings(5);
			ImGui::SameLine(200.0f - ImGui::CalcTextSize("Primrose").x / 2);
			ImGui::Text("Primrose");
			AddSpacings(5);
			AddSeparators(1);

			AddSpacings(5);
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

		ImGui::EndMainMenuBar();
	}
	

}
void Editor::RenderViewportWindow() {

	//NOTE: I dont necesseraly need the viewport window to do the drag and drop check but i kept it for now incase of future use
	//NOTE: The tabs code could be done easily by getting the payload nad checking if no window is hovered.

	ImGuiWindowFlags ViewportFlags = 0;
	ViewportFlags |= ImGuiWindowFlags_NoBackground;
	ViewportFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	ViewportFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
	bool Open = true;
	bool Hovered = false;
	bool LeftMouseReleased = false;
	if (ImGui::Begin("ViewportWindow", &Open, ViewportFlags)) {
		ImGui::SetWindowSize(m_GUIViewport->Size);
		ImGui::SetWindowPos(ImVec2(0, 0));


		//Tabs payload check - Gets tab payload and sets it to true
		auto Payload = ImGui::GetDragDropPayload();
		Hovered = ImGui::IsWindowHovered();
		LeftMouseReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
		if (Hovered && LeftMouseReleased && Payload != nullptr) {
			if (Payload->IsDataType("Tab")) {

				bool ContentBrowserCache = m_ContentBrowserOpenedStandalone;
				//bool DebugLogCache = m_DebugLogOpenedStandalone;
				//bool SystemLogCache = m_SystemLogOpenedStandalone;

				bool** DataPointer = static_cast<bool**>(Payload->Data);
				if (DataPointer != nullptr) {
					*(*DataPointer) = true; //Does not look so nice

					//Terrible solution
					if (m_ContentBrowserOpenedStandalone != ContentBrowserCache)
						m_ContentBrowserWindowPosition = ImGui::GetMousePos();
				}
			}
		}



		ImGui::End();
	}
}
void Editor::RenderStandaloneWindows() {


	if (m_ContentBrowserOpenedStandalone) {
		//Create Window and call appropriate function inside!
		//Check window if closed afterwards to toggle other stuff?

		if (ImGui::Begin("ContentBrowser", &m_ContentBrowserOpenedStandalone)) {
			ImGui::SetWindowPos(m_ContentBrowserWindowPosition);

		}

		ImGui::End();
	}
	if (m_DebugLogOpenedStandalone) {
		if (ImGui::Begin("DebugLog")) {



		}
		ImGui::End();
	}
	if (m_SystemLogOpenedStandalone) {
		if (ImGui::Begin("SystemLog")) {



		}
		ImGui::End();
	}



}

void Editor::RenderContentBrowser() {

	if (m_SelectedDirectory != nullptr) {
		AddSpacings(5);
		SetupContentBrowserStyle();
		NewContentBrowserFrame();
		UpdateContentBrowserFolderEntries();
		if (!m_FolderEntryOpened)
			UpdateContentBrowserAssetEntries();

		FlushContentTexts();
		ClearContentBrowserStyle();
	}
}
void Editor::RenderDebugLog() {

	if (ImGui::BeginMenuBar()) {

		//I guess it makes sense why they use static so much
		bool ClearSelected = false;
		const ImVec2 ClearSize = ImGui::CalcTextSize("Clear");
		const ImVec2 AutoScrollSize = ImGui::CalcTextSize("AutoScroll");
		if (ImGui::Selectable("Clear", &ClearSelected, 0, ClearSize)) {
			m_Logger.ClearMessageLog();
		}
		if (ImGui::Selectable("AutoScroll", m_Logger.GetAutoScrollDebugLog(), 0, AutoScrollSize)) {
			m_Logger.ToggleAutoScrollDebugLog();
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
		if (!m_Logger.GetShowErrorMessages())
			IconColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);


		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 30.0f, ImGui::GetCursorPosY())); //Manual offset by +2
		if (ImGui::ImageButton("##ErrorLog", TextureID, ImVec2(15.0f, 15.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), IconColor))
			m_Logger.ToggleShowErrorMessages();
		if (m_ErrorTexture != nullptr)
			m_ErrorTexture->Unbind();

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 5.0f, ImGui::GetCursorPosY()));
		ImGui::Text("%d", m_Logger.GetErrorMessagesCount());
		IconColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


		//Texture
		if (m_WarningTexture) {
			m_WarningTexture->Bind();
			TextureID = (void*)(intptr_t)(m_WarningTexture->GetID());
		}

		//Color
		if (!m_Logger.GetShowWarningMessages())
			IconColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 80.0f, ImGui::GetCursorPosY())); //Manual offset by +2
		if (ImGui::ImageButton("##WarningLog", TextureID, ImVec2(15.0f, 15.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), IconColor))
			m_Logger.ToggleShowWarningMessages();
		if (m_WarningTexture != nullptr)
			m_WarningTexture->Unbind();

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 55.0f, ImGui::GetCursorPosY()));
		ImGui::Text("%d", m_Logger.GetWarningMessagesCount());
		IconColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


		//Texture
		if (m_DebugTexture) {
			m_DebugTexture->Bind();
			TextureID = (void*)(intptr_t)(m_DebugTexture->GetID());
		}

		//Color
		if (!m_Logger.GetShowDebugMessages())
			IconColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 130.0f, ImGui::GetCursorPosY())); //Manual offset by +2
		if (ImGui::ImageButton("##DebugLog", TextureID, ImVec2(15.0f, 15.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), IconColor))
			m_Logger.ToggleShowDebugMessages();
		if (m_DebugTexture != nullptr)
			m_DebugTexture->Unbind();

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 105.0f, ImGui::GetCursorPosY()));
		ImGui::Text("%d", m_Logger.GetDebugMessagesCount());
		IconColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();


		ImGui::EndMenuBar();
	}

	if (m_Logger.GetLoggedMessagesCount() > 0) {

		while (!m_Logger.IsLoggedBufferLooped()) {

			const Message* NewMessage = m_Logger.GetNextLoggedMessage();
			if (NewMessage == nullptr)
				continue;

			const MessageType Type = NewMessage->GetType();
			if (Type == MessageType::DEBUG && !m_Logger.GetShowDebugMessages())
				continue;
			if (Type == MessageType::WARNING && !m_Logger.GetShowWarningMessages())
				continue;
			if (Type == MessageType::ERROR && !m_Logger.GetShowErrorMessages())
				continue;
			if (Type == MessageType::SYSTEM) {
				m_Logger.ErrorLog("System type message was saved in the logged messages buffer");
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

			ImGui::Text("[%s] [%llu] %s", m_TimeReference->FormatTime(NewMessage->GetTimestamp()).data(), NewMessage->GetTick(), NewMessage->GetData().data());

			ImGui::PopStyleColor();
			AddSeparators(1);
		}
	}

	//AutoScroll debug log
	if (ImGui::GetScrollY() <= ImGui::GetScrollMaxY() && m_Logger.GetAutoScrollDebugLog())
		ImGui::SetScrollHereY(1.0f);
}
void Editor::RenderSystemLog() {

}


void Editor::CheckInput() {

	//NOTE: Needs to be after the render calls otherwise its always false cause the flag is reseted right on top of this function

	//Unselect from hieracrchy
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && m_SelectedGameObject != nullptr && !m_IsAnyWindowHovered) {
		//The only problem with this is that the IsAnyWindowHovered will not catch windows if a popup is open
		SetSelectedGameObject(nullptr);
	}


	//Unfinished - NOTE: It might be worthwhile to save the name of the hovered over window
	////Deselect content element
	//if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && m_SelectedContentElement != nullptr && !ImGui::IsAnyItemHovered()) {
	//	m_SelectedContentElement = nullptr;
	//}

	//Delete selected from hieracrchy
	if (ImGui::IsKeyReleased(ImGuiKey_Delete)) {
		if (m_SelectedGameObject != nullptr) {
			m_SelectedGameObject->Destroy();
			SetSelectedGameObject(nullptr);
		}
	}
}
void Editor::UpdateViewportControls() {

	if (m_IsAnyWindowHovered)
		return;


	//NOTE: This function could be broken into 3 or 4 parts.

	if (m_InputReference->GetMouseKey(MouseKeyCode::RIGHT) && !m_ViewportNavigationMode) {
		m_ViewportNavigationMode = true;
		m_InputReference->SetMouseInputMode(MouseMode::DISABLED);
	}
	else if (!m_InputReference->GetMouseKey(MouseKeyCode::RIGHT) && m_ViewportNavigationMode) {
		m_ViewportNavigationMode = false;
		m_InputReference->SetMouseInputMode(MouseMode::NORMAL);
	}

	if (!m_ViewportNavigationMode) {
		return;
	}

	if (m_ViewportCameraReference == nullptr)
		return;

	const float DeltaTime = static_cast<float>(m_TimeReference->GetDeltaTime());


	if (m_InputReference->GetKey(Keycode::W)) {
		m_ViewportCameraReference->MoveY(m_CameraMovementSpeed * DeltaTime);
	}
	if (m_InputReference->GetKey(Keycode::S)) {
		m_ViewportCameraReference->MoveY((m_CameraMovementSpeed * DeltaTime) * -1);
	}
	if (m_InputReference->GetKey(Keycode::A)) {
		m_ViewportCameraReference->MoveX((m_CameraMovementSpeed * DeltaTime) * -1);
	}
	if (m_InputReference->GetKey(Keycode::D)) {
		m_ViewportCameraReference->MoveX(m_CameraMovementSpeed * DeltaTime);
	}
	if (m_InputReference->GetKey(Keycode::E)) {
		m_ViewportCameraReference->MoveVertical(m_CameraMovementSpeed * DeltaTime);
	}
	if (m_InputReference->GetKey(Keycode::Q)) {
		m_ViewportCameraReference->MoveVertical((m_CameraMovementSpeed * DeltaTime) * -1);
	}

	const Vector2f CursorDelta = m_InputReference->GetMouseCursorDelta();

	//TODO: Make funcs for those
	if (CursorDelta.m_X >= m_FreeLookSensitivity)
		m_ViewportCameraReference->RotateY(m_FreeLookSpeed * DeltaTime * CursorDelta.m_X);
	if (CursorDelta.m_X <= -m_FreeLookSensitivity)
		m_ViewportCameraReference->RotateY((m_FreeLookSpeed * DeltaTime * -CursorDelta.m_X) * -1);

	if (CursorDelta.m_Y >= m_FreeLookSensitivity)
		m_ViewportCameraReference->RotateX(m_FreeLookSpeed * DeltaTime * CursorDelta.m_Y);
	if (CursorDelta.m_Y <= -m_FreeLookSensitivity)
		m_ViewportCameraReference->RotateX((m_FreeLookSpeed * DeltaTime * -CursorDelta.m_Y) * -1);

	const float ScrollDelta = m_InputReference->GetScrollDelta();

	//Scroll Wheel
	if (ScrollDelta > 0.0f) {
		m_CameraMovementSpeed += m_CameraSpeedIncrease * DeltaTime;
		if (m_CameraMovementSpeed > m_CameraSpeedMax)
			m_CameraMovementSpeed = m_CameraSpeedMax;
	}
	else if (ScrollDelta < 0.0f) {
		m_CameraMovementSpeed -= m_CameraSpeedDecrease * DeltaTime;
		if (m_CameraMovementSpeed < m_CameraSpeedMin)
			m_CameraMovementSpeed = m_CameraSpeedMin;
	}
}


void Editor::AddHeirarchyEntry(GameObject* entry) {

	if (entry == nullptr)
		return;

	ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	bool IsLeaf = false;
	if (!entry->HasChildren()) {
		Flags |= ImGuiTreeNodeFlags_Leaf;
		IsLeaf = true;
	}

	if (entry == m_SelectedGameObject)
		Flags |= ImGuiTreeNodeFlags_Selected;

	uint32 PushedStyles = 0;
	if (!entry->GetActiveInHeirarchy()) {
		PushedStyles++;
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::TreeNodeEx(entry->GetName().data(), Flags)) {

		if (ImGui::IsItemClicked()) {
			SetSelectedGameObject(entry);
		}
		else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
			ImGui::OpenPopup("EditMenu");
			SetSelectedGameObject(entry);
		}

		//EditMenu
		bool Renaming = false; //Need this cause i think EndPopup() clears the rename popup id
		bool Deleting = false;
		if (ImGui::BeginPopup("EditMenu")) {

			ImGui::Text(std::string("Edit: " + entry->GetName()).data());

			if (ImGui::Selectable("Delete", false)) {
				ImGui::CloseCurrentPopup();
				Deleting = true;
			}
			if (ImGui::Selectable("Rename", false)) {
				ImGui::CloseCurrentPopup();
				Renaming = true;
			}

			ImGui::EndPopup();
		}

		//Deleting
		if (Deleting) {
			ImGui::OpenPopup("GODeleteConfirmation");
		}
		if (ImGui::BeginPopup("GODeleteConfirmation")) {

			ImGui::Text("Are you sure?");
			ImGui::Spacing();

			ImGui::PushItemWidth(40.0f);
			if (ImGui::Button("Yes")) {
				if (entry == m_SelectedGameObject) //Note: Any pointers to objects like these need to be reseted to nullptr somehow.
					SetSelectedGameObject(nullptr);
				//m_SelectedGameObject = nullptr;

				entry->Destroy();
				ImGui::EndPopup();
				ImGui::TreePop();
				return; //It would break otherwise?
			}
			ImGui::SameLine(80.0f);

			ImGui::PushItemWidth(40.0f);
			if (ImGui::Button("No")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		//Renaming
		if (Renaming) {
			strcpy_s(m_NameInputBuffer, entry->GetName().c_str());
			ImGui::OpenPopup("NameInput");
		}
		if (ImGui::BeginPopup("NameInput")) {
			if (ImGui::InputText("Name", m_NameInputBuffer, 32, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
				entry->SetName(m_NameInputBuffer);
			}

			ImGui::EndPopup();
		}

		//Parent-Child
		if (ImGui::BeginDragDropSource()) {
			auto PointerToReference = &entry;
			ImGui::SetDragDropPayload("HeirarchyEntry", PointerToReference, sizeof(PointerToReference));


			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget()) {

			auto Payload = ImGui::AcceptDragDropPayload("HeirarchyEntry");
			if (Payload != nullptr) {
				auto PointerToReference = static_cast<GameObject**>(Payload->Data);
				GameObject* PointerToEntry = *PointerToReference;
				entry->AddChild(PointerToEntry);
			}

			ImGui::EndDragDropTarget();
		}

		if (!IsLeaf) {
			if (PushedStyles > 0) {
				ImGui::PopStyleVar();
				PushedStyles--;
			}

			for (auto& x : entry->GetChildren())
				AddHeirarchyEntry(x);
		}

		ImGui::TreePop();
	}

	if (PushedStyles > 0) {
		ImGui::PopStyleVar();
		PushedStyles--;
	}
}
void Editor::AddFileExplorerEntry(Directory* entry) {

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
			SetSelectedDirectory(entry);
		}

		if (!IsLeaf) {
			for (auto& x : entry->m_Folders)
				AddFileExplorerEntry(x);
		}

		ImGui::TreePop();
	}

}


void Editor::NewContentBrowserFrame() noexcept {
	m_ContentElementCursor = 0.0f;
	m_ContentLineElementsCount = 0;
	m_FolderEntryOpened = false;
}
void Editor::UpdateContentBrowserFolderEntries() {

	//TODO: Implement some countermeasure in case the texture was not found
	//Bind texture
	void* TextureID = nullptr;
	if (m_FolderTexture) {
		m_FolderTexture->Bind();
		TextureID = (void*)(intptr_t)(m_FolderTexture->GetID());
	}

	for (auto& Folder : m_SelectedDirectory->m_Folders) {

		std::string Name = Folder->m_Path.filename().replace_extension().string();
		std::string ID = "##" + Name;
		bool AppliedStyle = false;

		//Calculate position and check if new line is necessary
		m_ContentElementCursor = (m_ContentBrowserElementPadding * (m_ContentLineElementsCount + 1)) + m_ContentBrowserElementSize.x * m_ContentLineElementsCount;
		if (m_ContentElementCursor + m_ContentBrowserElementSize.x >= m_ContentWindowSize.x) {
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
		m_QueuedContentTexts.emplace_back(Name);

		m_ContentLineElementsCount++;
	}

	//Unbind texture
	if (m_FolderTexture)
		m_FolderTexture->Unbind();
}
void Editor::UpdateContentBrowserAssetEntries() {

	for (auto& Asset : m_SelectedDirectory->m_Assets) {

		//TODO: Deal with in case it wasnt found!
		void* TextureID = nullptr;
		const Texture2D* IconTexture = GetIconTexture(*Asset);
		if (IconTexture != nullptr) {
			TextureID = (void*)(intptr_t)(IconTexture->GetID());
			IconTexture->Bind();
		}


		std::string Name = Asset->m_Path.filename().replace_extension().string();
		std::string ID = "##" + Name;
		bool AppliedStyle = false;

		//Calculate position and check if new line is necessary
		m_ContentElementCursor = (m_ContentBrowserElementPadding * (m_ContentLineElementsCount + 1)) + m_ContentBrowserElementSize.x * m_ContentLineElementsCount;
		if (m_ContentElementCursor + m_ContentBrowserElementSize.x >= m_ContentWindowSize.x) {

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
			if (m_SelectedContentElement == Asset) {
				m_SelectedContentElement = nullptr;
				//OPEN WHATEVER!
			}
			else
				m_SelectedContentElement = Asset;
		}

		//Pop selected style
		if (AppliedStyle)
			ImGui::PopStyleColor();

		m_QueuedContentTexts.emplace_back(Name);
		m_ContentLineElementsCount++;

		if (IconTexture != nullptr)
			IconTexture->Unbind();
	}
}
void Editor::FlushContentTexts() {

	if (m_QueuedContentTexts.size() == 0)
		return;

	AddSpacings(1);
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
	AddSpacings(3);
}


bool Editor::AddBlendModeSelectable(SourceBlendMode mode) {
	using namespace EnumToText;

	if (ImGui::Selectable(ToText(mode), false))
		return true;
	else
		return false;
}
bool Editor::AddBlendModeSelectable(DestinationBlendMode mode) {
	using namespace EnumToText;

	if (ImGui::Selectable(ToText(mode), false))
		return true;
	else
		return false;
}
bool Editor::AddAddressingModeSelectable(AddressingMode mode) {
	using namespace EnumToText;

	if (ImGui::Selectable(ToText(mode), false))
		return true;
	else
		return false;
}
bool Editor::AddFilteringModeMinSelectable(FilteringModeMin mode) {
	using namespace EnumToText;

	if (ImGui::Selectable(ToText(mode), false))
		return true;
	else
		return false;
}
bool Editor::AddFilteringModeMagSelectable(FilteringModeMag mode) {
	using namespace EnumToText;

	if (ImGui::Selectable(ToText(mode), false))
		return true;
	else
		return false;
}


void Editor::AddSpacings(uint32 count) {
	for (uint32 index = 0; index < count; index++)
		ImGui::Spacing();
}
void Editor::AddSeparators(uint32 count) {
	for (uint32 index = 0; index < count; index++)
		ImGui::Separator();
}
void Editor::PopStyleVars(uint32 count) {
	for (uint32 index = 0; index < count; index++)
		ImGui::PopStyleVar();
}
void Editor::SetSelectedGameObject(GameObject* object) noexcept {

	m_SelectedGameObject = object;
	if (m_SelectedGameObject != nullptr) {
		strcpy_s(m_NameInputBuffer, m_SelectedGameObject->GetName().c_str());
		strcpy_s(m_TagInputBuffer, m_SelectedGameObject->GetTag().c_str());
	}
}
void Editor::SetSelectedDirectory(Directory* directory) noexcept {

	m_SelectedDirectory = directory;
}
void Editor::CheckForHoveredWindows() {

	if (ImGui::IsWindowHovered())
		m_IsAnyWindowHovered = true;
}
Texture2D* Editor::GetIconTexture(const Asset& asset) {

	Texture2D* IconTexture;

	//TODO: Set the alpha blending for these icons cause transparency is borked currenrtly

	switch (asset.m_Type)
	{
	case AssetType::TEXTURE: {

		//TODO: Add name to the asset and remove it from Texture2D and update that everywhere! It makes more sense and all asset types will have it
		if (!m_TextureStorageReference->GetTexture2D(asset.m_Path.filename().replace_extension().string(), IconTexture))
			return nullptr;
		return IconTexture;
	}
	default:
		if (m_TextureStorageReference->GetTexture2D("Unknown", IconTexture))
			return IconTexture;
		return nullptr;
	}
}


void Editor::SetupEditorStyle() {

	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));

	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));

	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.05f, 0.0f, 0.0f, 1.0f));

	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));

	ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.5f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
}
void Editor::ClearEditorStyle() {

	//15 - Make sure its the same amount of PushStyleColor() calls in SetupEditorStyle();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}
void Editor::SetupContentBrowserStyle() {

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
}
void Editor::ClearContentBrowserStyle() {

	//3 - Make sure its the same amount of PushStyleColor() calls in SetupContentBrowserStyle();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

bool Editor::IsPointInBoundingBox(ImVec2 point, ImVec2 position, ImVec2 size) const noexcept {

	bool XWithin = false;
	bool YWithin = false;

	if (point.x >= position.x && point.x <= position.x + size.x)
		XWithin = true;
	if (point.y >= position.y && point.y <= position.y + size.y)
		YWithin = true;

	return XWithin &= YWithin;
}