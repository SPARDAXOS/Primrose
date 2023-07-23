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

	m_ViewportCameraReference = &m_ECSReference->GetViewportCamera();
	if (m_ViewportCameraReference == nullptr)
		PrintMessage("Failed to save viewport camera reference");

	//Get folder texture?

	IMGUI_CHECKVERSION();
	m_GUIContext = ImGui::CreateContext();
	m_GUIViewport = ImGui::GetMainViewport();


	//CalculateWindowsSizes()

	m_Logger.SetTimeReference(*m_TimeReference); //Since it will have nullptr on this class' construction


	for (uint32 count = 0; count < 40; count++) {
		m_Logger.LogDebug("Debug Message!");
		m_Logger.LogWarning("Warning Message!");
		m_Logger.LogError("Error Message!");
	}

	

	m_IO = &ImGui::GetIO();
	m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// IF using Docking Branch
	m_IO->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; //Disable IMGUI from interfering with glfw when it comes to cursor visibility and shape



	ImGui_ImplGlfw_InitForOpenGL(m_WindowReference->GetWindowResource().m_ptr, true);
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
		PrintMessage("Failed to save reference to engine texture [Folder]");

}


void Editor::Render() {//TODO: REturn const to this after getting rid of the text transform

	StartFrame();
	m_IsAnyWindowHovered = false;
	m_Logger.NewFrame();

	SetupEditorStyle();

	//IMPORTANT NOTE: Make the size of each element in relation to the size of the viewport so they would scale with it
	//TODO: All the menus sizes and positions are relative to each other. Make sure to make the values used relative to each other instead of relying on literals
	RenderDetailsMenu();
	RenderHeirarchyMenu();
	RenderAddGameObjectButton();
	RenderDirectoryExplorer();
	RenderContentBrowser();
	RenderMainMenuBar();



	//ImGui::ShowMetricsWindow(); Good for debugging

	ClearEditorStyle();

	RenderFrame();
}
void Editor::StartFrame() const {
	//Feed input and start GUI frame

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
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

	ImGui::Begin("Heirarchy", &m_HeirarchyWindowOpened, ImGuiWindowFlags_NoBringToFrontOnFocus);

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
void Editor::RenderContentBrowser() {

	if (!m_ContentBrowserWindowOpened)
		return;

	ImGuiWindowFlags ContentWindowFlags = 0;
	ContentWindowFlags |= ImGuiWindowFlags_NoCollapse;
	ContentWindowFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
	ContentWindowFlags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;
	//ContentWindowFlags |= ImGuiWindowFlags_NoResize;

	ImGuiWindowFlags TabsWindowFlags = 0;
	TabsWindowFlags |= ImGuiWindowFlags_NoCollapse;
	TabsWindowFlags |= ImGuiWindowFlags_NoTitleBar;
	TabsWindowFlags |= ImGuiWindowFlags_NoScrollbar;
	//TabsWindowFlags |= ImGuiWindowFlags_NoResize;
	TabsWindowFlags |= ImGuiWindowFlags_NoBackground;
	TabsWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;


	m_ContentBrowserWindowPosition = ImVec2(m_DirectoryExplorerWindowSize.x, m_GUIViewport->Size.y - m_ContentBrowserWindowSize.y);
	m_ContentBrowserTabsPosition.x = m_ContentBrowserWindowPosition.x;

	bool ContentBrowserOpened = false;
	bool LogOpened = false;
	std::string TabName = "";

	ImGui::SetNextWindowSize(m_ContentBrowserTabsSize);
	ImGui::SetNextWindowPos(m_ContentBrowserTabsPosition);
	if (ImGui::Begin("##TabsWindow", &m_ContentBrowserWindowOpened, TabsWindowFlags)) {
		CheckForHoveredWindows();

		if (ImGui::BeginTabBar("Tabs", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable)) {

			if (ImGui::BeginTabItem("ContentBrowser")) {
				ContentBrowserOpened = true;
				TabName = "Content";
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Log")) {
				LogOpened = true;
				TabName = "Logged Events";
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
	

	if (LogOpened)
		ContentWindowFlags |= ImGuiWindowFlags_MenuBar;

	ImGui::SetNextWindowSize(m_ContentBrowserWindowSize);
	ImGui::SetNextWindowPos(m_ContentBrowserWindowPosition);
	if (ImGui::Begin(TabName.data(), &m_ContentBrowserWindowOpened, ContentWindowFlags)) {
		CheckForHoveredWindows();

		//Its one frame behind so it looks like its animated (Lagging behind)
		m_ContentBrowserTabsPosition.y = m_GUIViewport->Size.y - (ImGui::GetWindowSize().y + m_ContentBrowserTabsSize.y);

		if (ContentBrowserOpened) {

			AddSpacings(5);
			if (m_SelectedDirectory != nullptr) {

				SetupContentBrowserStyle();
				NewContentBrowserFrame();
				UpdateContentBrowserFolderEntries();
				if (!m_FolderEntryOpened)
					UpdateContentBrowserAssetEntries();

				FlushContentTexts();
				ClearContentBrowserStyle();
			}

		}
		else if (LogOpened) {

			if (ImGui::BeginMenuBar()) {

				//I guess it makes sense why they use static so much
				bool FilterSelected = false;
				bool ClearSelected = false;
				const ImVec2 ClearSize = ImGui::CalcTextSize("Clear");
				const ImVec2 FilterSize = ImGui::CalcTextSize("Filter");
				if (ImGui::Selectable("Clear", &ClearSelected, 0, ClearSize)) {
					m_Logger.Clear();
				}
				if (ImGui::Selectable("Filter", &FilterSelected, 0, FilterSize)) {
					ImGui::OpenPopup("LoggerFilter");
					ImGui::SetNextWindowPos(ImGui::GetMousePos());
				}
				if (ImGui::BeginPopup("LoggerFilter")) {
					ImGui::Checkbox("Debug", &m_Logger.GetShowDebugMessagesRef());
					ImGui::Checkbox("Warning", &m_Logger.GetShowWarningMessagesRef());
					ImGui::Checkbox("Error", &m_Logger.GetShowErrorMessagesRef());

					ImGui::EndPopup();
				}

				ImGui::EndMenuBar();
			}

			if (m_Logger.GetAllMessagesCount() > 0) {

				while (!m_Logger.IsBufferLooped()) {

					const Message* NewMessage = m_Logger.GetNextMessage();
					if (NewMessage == nullptr)
						continue;

					const MessageType Type = NewMessage->GetType();
					if (Type == MessageType::DEBUG && !m_Logger.GetShowDebugMessages())
						continue;
					if (Type == MessageType::WARNING && !m_Logger.GetShowWarningMessages())
						continue;
					if (Type == MessageType::ERROR && !m_Logger.GetShowErrorMessages())
						continue;


					if (Type == MessageType::DEBUG)
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
					else if (Type == MessageType::WARNING)
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.1f, 1.0f));
					else if (Type == MessageType::ERROR)
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

					//TODO: Add icons and numbers to show count of each message type
					//TODO: Add visibility of each icon that indicates if the filter is active or not
					//TODO: Add more info to messages like running time - Check what Unity does


					ImGui::Text("%s\nTick: %lu", NewMessage->GetData(), NewMessage->GetTick());

					ImGui::PopStyleColor();
					AddSeparators(1);
				}
			}

		}

		ImGui::End();
	}

}
void Editor::RenderMainMenuBar() {

	if (ImGui::BeginMainMenuBar()) {



		//NOTE: Test BeginMenu and MenuItem for this!


		constexpr float BarPadding = 4.0f; //I did a bunch of calculations and used Photoshop to confirm this
		constexpr float ElementsPadding = 10.0f;
		float CurrentElementWidth = 0.0f;
		ImVec2 OpenedPopupPosition{0.0f, 0.0f};

		//Need to query height
		if (m_MainMenuBarSize.x == 0.0f && m_MainMenuBarSize.y == 0.0f)
			m_MainMenuBarSize = ImGui::GetWindowSize();

		OpenedPopupPosition.y = m_MainMenuBarSize.y;

		//File
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.0f));
		CurrentElementWidth = ImGui::CalcTextSize("File").x + ElementsPadding;
		if (ImGui::Selectable("File", false, 0, ImVec2(CurrentElementWidth, m_MainMenuBarSize.y)))
			ImGui::OpenPopup("FileMenu");

		PopStyleVars(1);
		if (ImGui::BeginPopup("FileMenu")) {

			ImGui::SetWindowPos(OpenedPopupPosition);

			bool Test1Selected = false;
			if (ImGui::Selectable("Test1Selected", &Test1Selected, 0, ImVec2(300.0f, 20.0f))) {

			}
			bool Test2Selected = false;
			if (ImGui::Selectable("Test2Selected", &Test2Selected, 0, ImVec2(300.0f, 20.0f))) {

			}
			bool Test3Selected = false;
			if (ImGui::Selectable("Test3Selected", &Test3Selected, 0, ImVec2(300.0f, 20.0f))) {

			}

			ImGui::EndPopup();
		}
		OpenedPopupPosition.x = ImGui::GetCursorPos().x - BarPadding;


		//Window
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.0f));
		CurrentElementWidth = ImGui::CalcTextSize("Window").x + ElementsPadding;
		if (ImGui::Selectable("Window", false, 0, ImVec2(CurrentElementWidth, m_MainMenuBarSize.y)))
			ImGui::OpenPopup("WindowMenu");
		
		PopStyleVars(1);
		if (ImGui::BeginPopup("WindowMenu")) {

			ImGui::SetWindowPos(OpenedPopupPosition);
			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.2f));

			bool DetailesSelected = false;
			if (ImGui::Selectable("Details", &DetailesSelected, 0, ImVec2(300.0f, 20.0f))) {
				m_DetailsWindowOpened = true;
			}
			bool HierarchySelected = false;
			if (ImGui::Selectable("Heirarchy", &HierarchySelected, 0, ImVec2(300.0f, 20.0f))) {
				m_HeirarchyWindowOpened = true;
			}
			bool ContentBrowserSelected = false;
			if (ImGui::Selectable("Content Browser", &ContentBrowserSelected, 0, ImVec2(300.0f, 20.0f))) {
				m_ContentBrowserWindowOpened = true;
			}
			bool DirectoryExplorerSelected = false;
			if (ImGui::Selectable("Directory Explorer", &DirectoryExplorerSelected, 0, ImVec2(300.0f, 20.0f))) {
				m_DirectoryExplorerWindowOpened = true;
			}

			ImGui::PopStyleVar();
			ImGui::EndPopup();
		}
		OpenedPopupPosition.x = ImGui::GetCursorPos().x - BarPadding;








		//About - Should be at the very end always!
		ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.0f)); //Middle - Top
		CurrentElementWidth = ImGui::CalcTextSize("About").x + ElementsPadding;
		if (ImGui::Selectable("About", false, 0, ImVec2(CurrentElementWidth, m_MainMenuBarSize.y)))
			ImGui::OpenPopup("AboutMenu");

		PopStyleVars(1);

		ImGuiWindowFlags Flags = 0;
		Flags |= ImGuiWindowFlags_NoTitleBar;
		Flags |= ImGuiWindowFlags_NoResize;
		Flags |= ImGuiWindowFlags_NoMove;
		Flags |= ImGuiWindowFlags_NoScrollbar;
		if (ImGui::BeginPopupModal("AboutMenu", 0, Flags)) {
			
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
		OpenedPopupPosition.x = ImGui::GetCursorPos().x;

		ImGui::EndMainMenuBar();
	}
	

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
		if (m_ContentElementCursor + m_ContentBrowserElementSize.x >= m_ContentBrowserWindowSize.x) {
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
		if (m_ContentElementCursor + m_ContentBrowserElementSize.x >= m_ContentBrowserWindowSize.x) {

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