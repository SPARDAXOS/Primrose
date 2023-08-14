#include "Systems/Editor.hpp"
#include "Systems/Core.hpp"



#include <iostream>


Editor::Editor(Core& core)
	: m_CoreReference(&core)
{
	m_WindowReference = m_CoreReference->GetWindow();
	m_ECSReference = m_CoreReference->GetECS();
	m_TextureStorageReference = m_CoreReference->GetTextureStorage();
	m_AssetManagerReference = m_CoreReference->GetAssetManager();
	m_InputReference = m_CoreReference->GetInput();
	m_TimeReference = m_CoreReference->GetTime();
	m_SerializerReference = m_CoreReference->GetSerializer();
	m_LoggerReference = m_CoreReference->GetLogger();
	//Get folder texture?

	IMGUI_CHECKVERSION();
	m_GUIContext = ImGui::CreateContext();
	m_GUIViewport = ImGui::GetMainViewport();
	//TODO: Throw if any of these dont work!

	m_LoggerReference->SystemLog("Dear IMGUI initialized.");
	std::string VersionMessage("Version: ");
	VersionMessage.append(ImGui::GetVersion());
	m_LoggerReference->SystemLog(VersionMessage);


	m_ViewportCameraReference = &m_ECSReference->GetViewportCamera();
	if (m_ViewportCameraReference == nullptr)
		m_LoggerReference->SystemLog("Failed to save viewport camera reference");


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

	if (!m_TextureStorageReference->GetEditorTexture2DByName("Folder", m_FolderTexture))
		m_LoggerReference->SystemLog("Failed to save reference to engine texture [Folder]");

	if (!m_TextureStorageReference->GetEditorTexture2DByName("Debug", m_DebugTexture))
		m_LoggerReference->SystemLog("Failed to save reference to engine texture [Debug]");

	if (!m_TextureStorageReference->GetEditorTexture2DByName("Warning", m_WarningTexture))
		m_LoggerReference->SystemLog("Failed to save reference to engine texture [Warning]");

	if (!m_TextureStorageReference->GetEditorTexture2DByName("Error", m_ErrorTexture))
		m_LoggerReference->SystemLog("Failed to save reference to engine texture [Error]");

}

void Editor::Render() {

	StartFrame();
	m_IsAnyWindowHovered = false;
	m_LoggerReference->NewFrame();

	UpdateWindowPositions(); //NOTE: Currently updates sizes and positions
	m_EditorStyle.Apply();

	//Testing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Physics* m_Physics = m_CoreReference->GetPhysics();
	Vector3f Test 
		= m_Physics->ScreenToWorld(Vector2f(static_cast<float>(m_InputReference->CursorX), static_cast<float>(m_InputReference->CursorY)), 0.0f);

	std::cout << "X: " << Test.m_X << " Y: " << Test.m_Y << " Z: " << Test.m_Z << std::endl;


	//Main Docking Space
	ImGuiDockNodeFlags Flags = 0;
	Flags |= ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(m_GUIViewport, Flags);


	//ImGui::ShowDemoWindow();
	//IMPORTANT NOTE: Make the size of each element in relation to the size of the viewport so they would scale with it
	//TODO: All the menus sizes and positions are relative to each other. Make sure to make the values used relative to each other instead of relying on literals
	RenderDetailsMenu();
	RenderHeirarchyMenu();
	RenderAddGameObjectMenu();
	RenderDirectoryExplorer();
	RenderViewportWindow();
	RenderMainMenuBar();
	RenderContentWindows();




	m_EditorStyle.Clear();

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


void Editor::RenderDetailsMenu() { 

	if (!m_DetailsWindowOpened)
		return;


	//TODO: Add the ability to reorder these one day


	//Render GUI
	//Note: Will also lock position and size - Use any func without the next to free it
	ImGui::SetNextWindowSize(m_DetailsWindowSize);
	ImGui::SetNextWindowPos(m_DetailsWindowPosition);

	ImGui::Begin("Details", &m_DetailsWindowOpened);
	CheckForHoveredWindows();

	if (m_SelectedGameObject != nullptr) {

		//TODO: Most of the elements positions in these functions need to be redone. They are hardcoded. 
		RenderInfoDetails();
		RenderTransformDetails();
		RenderSpriteRendererDetails();


		//Always at the bottom
		RenderAddComponentMenu();
	}

	ImGui::End();
}
void Editor::RenderHeirarchyMenu() {

	if (!m_HeirarchyWindowOpened)
		return;

	ImGui::SetNextWindowSize(m_HierarchyWindowSize);
	ImGui::SetNextWindowPos(m_HierarchyWindowPosition); 

	//ImGuiWindowFlags_NoBringToFrontOnFocus
	ImGui::Begin("Heirarchy", &m_HeirarchyWindowOpened);

	CheckForHoveredWindows();

	//TODO: This needs cleanup, especially the id check down there. Like test if the ID is reserved or something
	std::vector<GameObject*> GameObjects = m_ECSReference->GetGameObjects();
	for (uint32 i = 0; i < GameObjects.size(); i++) {
		if (!m_ECSReference->IsReserved(GameObjects.at(i)->GetObjectID()) && GameObjects.at(i)->GetParent() == nullptr)
			AddHeirarchyEntry(GameObjects.at(i));
	}

	ImGui::End();
	
}
void Editor::RenderAddGameObjectMenu() {

	if (!m_HeirarchyWindowOpened)
		return;

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoTitleBar;
	Flags |= ImGuiWindowFlags_NoResize;
	Flags |= ImGuiWindowFlags_NoMove;
	Flags |= ImGuiWindowFlags_NoScrollbar;
	Flags |= ImGuiWindowFlags_NoBackground;
	Flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	Flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	
	//NOTE: This whole calculation thing here needs to be redone.

	ImGui::SetNextWindowSize(ImVec2(100.0f, 0.0f)); //As big as the button
	ImGui::SetNextWindowPos(ImVec2(m_HierarchyWindowPosition.x + m_HierarchyWindowSize.x - 25.0f, -7.5f + m_MainMenuBarSize.y)); //BUG: Even after all this hardcoding this is still slightly off. It is visible
	ImGui::Begin("##AddGameObjectMenu", &m_HeirarchyWindowOpened, Flags);
	CheckForHoveredWindows();

	if (ImGui::Button("##AddGameObjectMenuButton", ImVec2(70.0f, 19.0f))) {
		const ImVec2 PopupPosition = { ImGui::GetMousePos().x - 75.0f, ImGui::GetMousePos().y }; //75 - Half box width
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
	Flags |= ImGuiWindowFlags_MenuBar;

	ImGui::Begin("Directory Explorer", &m_DirectoryExplorerWindowOpened, Flags);
	CheckForHoveredWindows();


	if (ImGui::BeginMenuBar()) {
		//Editor Filter
		ImGui::SetWindowFontScale(0.95f);
		const auto TextSize = ImGui::CalcTextSize("Editor");
		ImGui::Selectable("Editor", &m_DirectoryExplorerEditorFilter, 0, TextSize);
		ImGui::SetWindowFontScale(1.0f);


		ImGui::EndMenuBar();
	}



	auto ProjectRoot = m_AssetManagerReference->GetProjectRoot();
	if (ProjectRoot == nullptr) {
		m_LoggerReference->SystemLog("m_AssetManagerReference->GetProjectRoot() returned null! - RenderDirectoryExplorer()");
		return;
	}
	if (m_SelectedDirectory == nullptr) //Set default directory if not set
		m_SelectedDirectory = ProjectRoot;
	AddFileExplorerEntry(ProjectRoot);

	//TODO: Could be enabled by filter
	//TODO: Do something about the assets then cause they will not be loaded from the correct texture storage for the editor. Content Browser!

	if (m_DirectoryExplorerEditorFilter) {
		auto EditorRoot = m_AssetManagerReference->GetEditorRoot();
		if (EditorRoot == nullptr) {
			m_LoggerReference->SystemLog("m_AssetManagerReference->GetEditorRoot() returned null! - RenderDirectoryExplorer()");
			return;
		}
		AddFileExplorerEntry(EditorRoot);
	}


	ImGui::End();
}
void Editor::RenderInfoDetails() {

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
}
void Editor::RenderTransformDetails() {

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
}
void Editor::RenderSpriteRendererDetails() {

	if (m_SelectedGameObject->HasComponent<SpriteRenderer>()) {
		SpriteRenderer* SelectedSpriteRenderer = m_SelectedGameObject->GetComponent<SpriteRenderer>();
		if (ImGui::CollapsingHeader("SpriteRenderer", ImGuiTreeNodeFlags_DefaultOpen)) {
			
			//NOTE: These could be broken down into smaller functions as well

			//SPRITE
			//IMPORTANT NOTE: For code like this, i used the same color from the style but in case of colors that are not 1.0.0, 0.1.0 and 0.0.1 -
			//- the multiplication will not have the same results. So, in the long term, these values should be member variables and specific in the -
			//- editor style class.
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(m_EditorStyle.m_MainColor.m_R * 0.2f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(m_EditorStyle.m_MainColor.m_R * 0.3f, 0.0f, 0.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(m_EditorStyle.m_MainColor.m_R * 0.4f, 0.0f, 0.0f, 1.0f));

			const ImVec2 CursorPosition = ImGui::GetCursorPos();
			if (ImGui::Button("##SpriteNameButton", ImVec2(m_DetailsWindowSize.x * 0.3f, 15.0f))) {
				m_SpriteSelectorOpened = true;
				SetSpriteSelectorTarget(SelectedSpriteRenderer->GetSpriteRef()); //Here instead
			}
			ImGui::SetCursorPos(CursorPosition);
			const Texture2D* Sprite = SelectedSpriteRenderer->GetSprite();
			if (Sprite != nullptr)
				ImGui::Text(Sprite->GetName().data());
			else
				ImGui::Text("None");

			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar();

			//Add Material Selector


			AddSpacings(2);



			//TINT
			Color CurrentColor = SelectedSpriteRenderer->GetTint();
			if (ImGui::ColorEdit4("Tint", &CurrentColor.m_R, ImGuiColorEditFlags_NoInputs)) {
				SelectedSpriteRenderer->SetTint(CurrentColor);
			}

			AddSpacings(2);

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

			AddSpacings(2);


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


			AddSpacings(2);

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


			AddSpacings(2);

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
void Editor::RenderAddComponentMenu() {

	ImGui::Separator();
	AddSpacings(6);
	ImGui::SameLine((m_DetailsWindowSize.x / 2) - 75.0f);
	if (ImGui::Button("Add Component", ImVec2(150.0f, 20.0f))) {
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
			if (ImGui::MenuItem("Content Browser", "CTRL + D", &m_ContentBrowserOpened))
				m_ContentBrowserWindowReset = true;
			ImGui::MenuItem("Directory Explorer", "CTRL + D", &m_DirectoryExplorerWindowOpened);

			if (ImGui::BeginMenu("Logs")) {

				if (ImGui::MenuItem("Debug Log", "CTRL + D", &m_DebugLogOpened))
					m_DebugLogWindowReset = true;
				if (ImGui::MenuItem("System Log", "CTRL + D", &m_SystemLogOpened))
					m_SystemLogWindowReset = true;

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
	ViewportFlags |= ImGuiWindowFlags_NoSavedSettings;
	ViewportFlags |= ImGuiWindowFlags_NoMove;
	ViewportFlags |= ImGuiWindowFlags_NoTitleBar;
	ViewportFlags |= ImGuiWindowFlags_NoDocking;
	bool Open = true; //? should i allow the viewport to be closable. Yes in the future.
	if (ImGui::Begin("ViewportWindow", &Open, ViewportFlags)) {
		ImGui::SetWindowSize(m_GUIViewport->Size);
		ImGui::SetWindowPos(ImVec2(0, 0));




	}

	ImGui::End();
}
void Editor::RenderContentWindows() {
	//Change Names? 

	RenderContentBrowser();
	RenderDebugLog();
	RenderSystemLog();

	//Doesnt scale according to the rest of the windows
	RenderSpriteSelector();
	RenderMaterialEditor();
}

void Editor::RenderContentBrowser() {

	m_IsContentBrowserWindowHovered = false;
	if (!m_ContentBrowserOpened)
		return;

	bool m_IsOtherContentWindowOpened = false;
	m_IsOtherContentWindowOpened |= m_DebugLogOpened;
	m_IsOtherContentWindowOpened |= m_SystemLogOpened;

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_NoSavedSettings;


	//IMPORTANT NOTE: When it comes to the text offeset bug, check the alignment on the text by PushStyleVar(). update: that didnt do it i think.

	if (m_ContentBrowserWindowReset) {
		m_ContentBrowserWindowReset = false;

		if (!m_IsOtherContentWindowOpened) {
			ImGui::SetNextWindowPos(m_ContentBrowserWindowPosition);
			ImGui::SetNextWindowSize(m_ContentBrowserWindowSize);
		}
		else {
			ImGui::SetNextWindowPos(GetUniqueScreenCenterPoint(m_NewContentWindowSize));
			ImGui::SetNextWindowSize(m_NewContentWindowSize);
		}
	}

	if (ImGui::Begin("Content Browser", &m_ContentBrowserOpened, Flags)) {
		CheckForHoveredWindows();
		if (ImGui::IsWindowHovered())
			m_IsContentBrowserWindowHovered = true;

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

		m_ContentBrowserWindowSize = ImGui::GetWindowSize();

	}
	ImGui::End();

	UpdateContentBrowserMenu();
}
void Editor::RenderDebugLog() {

	if (!m_DebugLogOpened)
		return;

	bool m_IsOtherContentWindowOpened = false;
	m_IsOtherContentWindowOpened |= m_ContentBrowserOpened;
	m_IsOtherContentWindowOpened |= m_SystemLogOpened;

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_MenuBar;
	Flags |= ImGuiWindowFlags_NoSavedSettings;

	if (m_DebugLogWindowReset) {
		m_DebugLogWindowReset = false;

		if (!m_IsOtherContentWindowOpened) {
			ImGui::SetNextWindowPos(m_ContentBrowserWindowPosition);
			ImGui::SetNextWindowSize(m_ContentBrowserWindowSize);
		}
		else {
			ImGui::SetNextWindowSize(m_NewContentWindowSize);
			ImGui::SetNextWindowPos(GetUniqueScreenCenterPoint(m_NewContentWindowSize));
		}
	}

	if (ImGui::Begin("Debug Log", &m_DebugLogOpened, Flags)) {
		CheckForHoveredWindows();

		//TODO: These could be broken down into functions
		//TODO: The auto scroll could be made its own resusable function
		if (ImGui::BeginMenuBar()) {

			//I guess it makes sense why they use static so much
			bool ClearSelected = false;
			const ImVec2 ClearSize = ImGui::CalcTextSize("Clear");
			const ImVec2 AutoScrollSize = ImGui::CalcTextSize("AutoScroll");
			if (ImGui::Selectable("Clear", &ClearSelected, 0, ClearSize)) {
				m_LoggerReference->ClearDebugLog();
			}
			if (ImGui::Selectable("AutoScroll", m_LoggerReference->GetDebugLogAutoScroll(), 0, AutoScrollSize)) {
				m_LoggerReference->ToggleDebugLogAutoScroll();
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
			if (!m_LoggerReference->GetShowErrorMessages())
				IconColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);


			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 30.0f, ImGui::GetCursorPosY())); //Manual offset by +2
			if (ImGui::ImageButton("##ErrorLog", TextureID, ImVec2(15.0f, 15.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), IconColor))
				m_LoggerReference->ToggleShowErrorMessages();
			if (m_ErrorTexture != nullptr)
				m_ErrorTexture->Unbind();

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 5.0f, ImGui::GetCursorPosY()));
			ImGui::Text("%d", m_LoggerReference->GetErrorMessagesCount());
			IconColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


			//Texture
			if (m_WarningTexture) {
				m_WarningTexture->Bind();
				TextureID = (void*)(intptr_t)(m_WarningTexture->GetID());
			}

			//Color
			if (!m_LoggerReference->GetShowWarningMessages())
				IconColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 80.0f, ImGui::GetCursorPosY())); //Manual offset by +2
			if (ImGui::ImageButton("##WarningLog", TextureID, ImVec2(15.0f, 15.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), IconColor))
				m_LoggerReference->ToggleShowWarningMessages();
			if (m_WarningTexture != nullptr)
				m_WarningTexture->Unbind();

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 55.0f, ImGui::GetCursorPosY()));
			ImGui::Text("%d", m_LoggerReference->GetWarningMessagesCount());
			IconColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


			//Texture
			if (m_DebugTexture) {
				m_DebugTexture->Bind();
				TextureID = (void*)(intptr_t)(m_DebugTexture->GetID());
			}

			//Color
			if (!m_LoggerReference->GetShowDebugMessages())
				IconColor = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 130.0f, ImGui::GetCursorPosY())); //Manual offset by +2
			if (ImGui::ImageButton("##DebugLog", TextureID, ImVec2(15.0f, 15.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), ImVec4(0.0f, 0.0f, 0.0f, 0.0f), IconColor))
				m_LoggerReference->ToggleShowDebugMessages();
			if (m_DebugTexture != nullptr)
				m_DebugTexture->Unbind();

			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowContentRegionMax().x - 105.0f, ImGui::GetCursorPosY()));
			ImGui::Text("%d", m_LoggerReference->GetDebugMessagesCount());
			IconColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);


			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();


			ImGui::EndMenuBar();
		}
		if (m_LoggerReference->GetLoggedMessagesCount() > 0) {

			while (!m_LoggerReference->IsLoggedBufferLooped()) {

				const Message* NewMessage = m_LoggerReference->GetNextLoggedMessage();
				if (NewMessage == nullptr)
					continue;

				const MessageType Type = NewMessage->GetType();
				if (Type == MessageType::DEBUG && !m_LoggerReference->GetShowDebugMessages())
					continue;
				if (Type == MessageType::WARNING && !m_LoggerReference->GetShowWarningMessages())
					continue;
				if (Type == MessageType::ERROR && !m_LoggerReference->GetShowErrorMessages())
					continue;
				if (Type == MessageType::SYSTEM) {
					m_LoggerReference->ErrorLog("System type message was saved in the logged messages buffer");
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

				//AutoScroll
				if (ImGui::GetScrollY() <= ImGui::GetScrollMaxY() && m_LoggerReference->GetDebugLogAutoScroll())
					ImGui::SetScrollHereY(1.0f);
			}
		}
	}
	ImGui::End();
}
void Editor::RenderSystemLog() {

	if (!m_SystemLogOpened)
		return;

	bool m_IsOtherContentWindowOpened = false;
	m_IsOtherContentWindowOpened |= m_ContentBrowserOpened;
	m_IsOtherContentWindowOpened |= m_DebugLogOpened;

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_MenuBar;
	Flags |= ImGuiWindowFlags_NoSavedSettings;

	if (m_SystemLogWindowReset) {
		m_SystemLogWindowReset = false;

		if (!m_IsOtherContentWindowOpened) {
			ImGui::SetNextWindowPos(m_ContentBrowserWindowPosition);
			ImGui::SetNextWindowSize(m_ContentBrowserWindowSize);
		}
		else {
			ImGui::SetNextWindowSize(m_NewContentWindowSize);
			ImGui::SetNextWindowPos(GetUniqueScreenCenterPoint(m_NewContentWindowSize));
		}
	}

	if (ImGui::Begin("System Log", &m_SystemLogOpened, Flags)) {
		CheckForHoveredWindows();

		//NOTE: Change the namings in the logger class
		//-Debug should be the catagory and maybe just message or something instead of it
		//System is also far too general

		if (ImGui::BeginMenuBar()) {

			bool ClearSelected = false;
			const ImVec2 ClearSize = ImGui::CalcTextSize("Clear");
			const ImVec2 AutoScrollSize = ImGui::CalcTextSize("AutoScroll");
			if (ImGui::Selectable("Clear", &ClearSelected, 0, ClearSize))
				m_LoggerReference->ClearSystemLog();
			if (ImGui::Selectable("AutoScroll", m_LoggerReference->GetSystemLogAutoScroll(), 0, AutoScrollSize))
				m_LoggerReference->ToggleSystemLogAutoScroll();



			ImGui::EndMenuBar();
		}
		if (m_LoggerReference->GetSystemMessagesCount() > 0) {

			while (!m_LoggerReference->IsSystemBufferLooped()) {

				const Message* NewMessage = m_LoggerReference->GetNextSystemMessage();
				if (NewMessage == nullptr)
					continue;

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImGui::Text("[%s] [%llu] %s", m_TimeReference->FormatTime(NewMessage->GetTimestamp()).data(), NewMessage->GetTick(), NewMessage->GetData().data());
				ImGui::PopStyleColor();
				AddSeparators(1);

				//TODO: Moving the scroll manually disables auto scroll
				//AutoScroll debug log
				if (ImGui::GetScrollY() <= ImGui::GetScrollMaxY() && m_LoggerReference->GetSystemLogAutoScroll())
					ImGui::SetScrollHereY(1.0f);
			}
		}

	}
	ImGui::End();
}
void Editor::RenderSpriteSelector() {

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
		CheckForHoveredWindows();
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
void Editor::RenderMaterialEditor() {

	if (!m_MaterialEditorOpened || m_MaterialEditorTarget == nullptr)
		return;

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoCollapse;
	Flags |= ImGuiWindowFlags_NoSavedSettings;


	if (m_MaterialEditorWindowReset) {
		m_MaterialEditorWindowReset = false;
		ImGui::SetNextWindowSize(m_MaterialEditorWindowSize);
		ImGui::SetNextWindowPos(GetUniqueScreenCenterPoint(m_MaterialEditorWindowSize));
	}


	if (ImGui::Begin("Material Editor", &m_MaterialEditorOpened, Flags)) {
		CheckForHoveredWindows();

		//Style for texture selectors
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(m_EditorStyle.m_MainColor.m_R * 0.2f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(m_EditorStyle.m_MainColor.m_R * 0.3f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(m_EditorStyle.m_MainColor.m_R * 0.4f, 0.0f, 0.0f, 1.0f));

		ImVec2 TextureTypeTextSize;
		ImVec2 TextureNameTextSize;
		ImVec2 SpriteSelectorSize;
		std::string TextureName;
		ImVec2 NoneTextSize;

		NoneTextSize = ImGui::CalcTextSize("None");

		//TODO: When deleting materials. Check if it is selected!. Make general function for that stuff! 
		//-It checks if it is a selected asset/material/gameobject then does the appropriate thing like closing an editor or something.


		//////////
		//Diffuse
		//////////

		//Texture
		ImGui::SetCursorPos(ImVec2(m_MaterialEditorWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		TextureTypeTextSize = ImGui::CalcTextSize("Diffuse");
		ImGui::Text("Diffuse");
		ImGui::SameLine(TextureTypeTextSize.x + (m_MaterialEditorWindowSize.x * 0.1f));
		if (m_MaterialEditorTarget->m_Diffuse != nullptr) {
			TextureName = m_MaterialEditorTarget->m_Diffuse->GetName();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = NoneTextSize;
		}

		SpriteSelectorSize = ImVec2(TextureNameTextSize.x + (m_MaterialEditorWindowSize.x * 0.2f), 20.0f);
		if (ImGui::Button(TextureName.data(), SpriteSelectorSize)) {
			m_SpriteSelectorOpened = true;
			SetSpriteSelectorTarget(m_MaterialEditorTarget->m_Diffuse);
		}
		ImGui::Separator();


		//////////
		//Ambient
		//////////
		
		//Texture
		ImGui::SetCursorPos(ImVec2(m_MaterialEditorWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		TextureTypeTextSize = ImGui::CalcTextSize("Ambient");
		ImGui::Text("Ambient");
		ImGui::SameLine(TextureTypeTextSize.x + (m_MaterialEditorWindowSize.x * 0.1f));
		if (m_MaterialEditorTarget->m_Ambient != nullptr) {
			TextureName = m_MaterialEditorTarget->m_Ambient->GetName();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = NoneTextSize;
		}

		SpriteSelectorSize = ImVec2(TextureNameTextSize.x + (m_MaterialEditorWindowSize.x * 0.2f), 20.0f);
		if (ImGui::Button(TextureName.data(), SpriteSelectorSize)) {
			m_SpriteSelectorOpened = true;
			SetSpriteSelectorTarget(m_MaterialEditorTarget->m_Ambient);
		}

		//Strength
		ImGui::SetCursorPos(ImVec2(m_MaterialEditorWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		ImGui::Text("Strength");
		ImGui::SameLine(ImGui::CalcTextSize("Strength").x + (m_MaterialEditorWindowSize.x * 0.1f));
		ImGui::SetNextItemWidth(TextureNameTextSize.x + (m_MaterialEditorWindowSize.x * 0.2f));
		ImGui::InputFloat("##AmbientStrength", &m_MaterialEditorTarget->m_AmbientStrength);
		ImGui::Separator();


		//NOTE: Check what flags i get.


		//////////
		//Specular
		//////////
		
		//Texture
		ImGui::SetCursorPos(ImVec2(m_MaterialEditorWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		TextureTypeTextSize = ImGui::CalcTextSize("Specular");
		ImGui::Text("Specular");
		ImGui::SameLine(TextureTypeTextSize.x + (m_MaterialEditorWindowSize.x * 0.1f));
		if (m_MaterialEditorTarget->m_Specular != nullptr) {
			TextureName = m_MaterialEditorTarget->m_Specular->GetName();
			TextureNameTextSize = ImGui::CalcTextSize(TextureName.data());
		}
		else {
			TextureName = "None";
			TextureNameTextSize = NoneTextSize;
		}


		SpriteSelectorSize = ImVec2(TextureNameTextSize.x + (m_MaterialEditorWindowSize.x * 0.2f), 20.0f);
		if (ImGui::Button(TextureName.data(), SpriteSelectorSize)) {
			m_SpriteSelectorOpened = true;
			SetSpriteSelectorTarget(m_MaterialEditorTarget->m_Specular);
		}

		//Strength
		ImGui::SetCursorPos(ImVec2(m_MaterialEditorWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		ImGui::Text("Strength");
		ImGui::SameLine(ImGui::CalcTextSize("Strength").x + (m_MaterialEditorWindowSize.x * 0.1f));
		ImGui::SetNextItemWidth(TextureNameTextSize.x + (m_MaterialEditorWindowSize.x * 0.2f));
		ImGui::InputFloat("##AmbientStrength", &m_MaterialEditorTarget->m_SpecularStrength);

		//Shininess
		ImGui::SetCursorPos(ImVec2(m_MaterialEditorWindowSize.x * 0.05f, ImGui::GetCursorPos().y));
		ImGui::Text("Shininess");
		ImGui::SameLine(ImGui::CalcTextSize("Shininess").x + (m_MaterialEditorWindowSize.x * 0.1f));
		ImGui::SetNextItemWidth(TextureNameTextSize.x + (m_MaterialEditorWindowSize.x * 0.2f));
		ImGui::InputInt("##Shininess", &m_MaterialEditorTarget->m_SpecularShininess);
		ImGui::Separator();


		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);

		m_MaterialEditorWindowSize = ImGui::GetWindowSize();
	}
	ImGui::End();
}


void Editor::CheckInput() {

	//NOTE: Needs to be after the render calls otherwise its always false cause the flag is reseted right on top of this function

	//TODO: Deal with popups problem for ishovering check.

	//Unselect from hieracrchy
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && m_SelectedGameObject != nullptr && !m_IsAnyWindowHovered) {
		//The only problem with this is that the IsAnyWindowHovered will not catch windows if a popup is open
		SetSelectedGameObject(nullptr);
	}

	if (m_IsContentBrowserWindowHovered && !ImGui::IsAnyItemHovered()){

		if (m_SelectedContentElement != nullptr && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			m_SelectedContentElement = nullptr;

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
			m_CoreReference->ErrorLog("Released");
			//Short afterimage when deleting!
			m_OpenContentBrowserEditMenu = true;
		}
	}


	//Delete selected from hieracrchy
	if (ImGui::IsKeyReleased(ImGuiKey_Delete)) {
		
		//if (m_SelectedContentElement != nullptr) {
		//	//Shit...
		//}
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
				Deleting = true;
			}
			if (ImGui::Selectable("Rename", false)) {
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

	//TODO: Maybe move these in and try to minimize these weird variables being member variables
	m_ContentElementCursor = 0.0f;
	m_ContentLineElementsCount = 0;
	m_FolderEntryOpened = false;
}
void Editor::NewSpriteSelectorFrame() noexcept {
	m_SpriteSelectorElementCursor = 0.0f;
	m_SpriteSelectorLineElementsCount = 0;
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

		std::string ID = "##" + Folder->m_Name;
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
					m_AssetManagerReference->RemoveDirectory(*Folder);

				ImGui::EndPopup();
			}
		}
	}

	//Unbind texture
	if (m_FolderTexture)
		m_FolderTexture->Unbind();
}
void Editor::UpdateContentBrowserAssetEntries() {

	for (auto& Asset : m_SelectedDirectory->m_Assets) {

		//TODO: Deal with in case it wasnt found! - Doesnt the function do that?
		void* TextureID = nullptr;
		const Texture2D* IconTexture = GetIconTexture(*Asset);
		if (IconTexture != nullptr) {
			TextureID = (void*)(intptr_t)(IconTexture->GetID());
			m_TextureStorageReference->SetActiveTextureUnit(TextureUnit::TEXTURE0);
			IconTexture->Bind();
		}


		std::string ID = "##" + Asset->m_Name;
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

			//Select
			if (m_SelectedContentElement == Asset) {
				m_SelectedContentElement = nullptr;
				OpenAsset(*Asset);
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
					m_AssetManagerReference->RemoveAsset(*Asset);

				//TODO: Clean up to avoid after image

				ImGui::EndPopup();
			}
		}
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


void Editor::OpenAsset(Asset& asset) {

	//Add more types here!
	if (asset.m_Type == AssetType::MATERIAL) {
		m_MaterialEditorOpened = true;
		m_MaterialEditorWindowReset = true;
		SetMaterialEditorTarget(m_AssetManagerReference->GetMaterial(asset));
	}



}

void Editor::UpdateSpriteSelectorEntries() {

	//NOTE: This applies to the content browser as well but this could be cleaned a lot more than this

	AddSpacings(5);
	SetupContentBrowserStyle();
	NewSpriteSelectorFrame();

	for (auto& Texture : m_TextureStorageReference->GetTexture2DStorage()) {


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

	ClearContentBrowserStyle();
	FlushSpriteSelectorTexts();
}
void Editor::UpdateContentBrowserMenu() {

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
			m_AssetManagerReference->CreateNewFolder(*m_SelectedDirectory);
		}
		else if (ImGui::MenuItem("Material")) {
			m_AssetManagerReference->CreateAsset(AssetType::MATERIAL, *m_SelectedDirectory);
		}

		ImGui::EndPopup();
	}
}
void Editor::FlushSpriteSelectorTexts() {

	if (m_QueuedSpriteSelectorTexts.size() == 0)
		return;

	AddSpacings(1);
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
void Editor::SetSelectedGameObject(GameObject* object) noexcept {

	m_SelectedGameObject = object;
	if (m_SelectedGameObject != nullptr) {
		strcpy_s(m_NameInputBuffer, m_SelectedGameObject->GetName().c_str());
		strcpy_s(m_TagInputBuffer, m_SelectedGameObject->GetTag().c_str());
	}
	if (m_SpriteSelectorTarget != nullptr)
		m_SpriteSelectorTarget = nullptr;
}
void Editor::SetSelectedDirectory(Directory* directory) noexcept {

	m_SelectedDirectory = directory;
}
void Editor::CheckForHoveredWindows() {

	if (ImGui::IsWindowHovered())
		m_IsAnyWindowHovered = true;
}
Texture2D* Editor::GetIconTexture(const Asset& asset) noexcept {

	Texture2D* IconTexture;

	//TODO: Set the alpha blending for these icons cause transparency is borked currenrtly.

	switch (asset.m_Type)
	{
	case AssetType::TEXTURE: {

		if (asset.m_EditorAsset) {
			if (!m_TextureStorageReference->GetEditorTexture2DByName(asset.m_Name, IconTexture)) {
				if (!m_TextureStorageReference->GetEditorTexture2DByName("Error", IconTexture))
					return nullptr;
				return IconTexture;
			}
		}
		else {
			if (!m_TextureStorageReference->GetTexture2DByName(asset.m_Name, IconTexture)) {
				if (!m_TextureStorageReference->GetEditorTexture2DByName("Error", IconTexture))
					return nullptr;
				return IconTexture;
			}
		}

		return IconTexture;
	}break;
	case AssetType::MATERIAL: {

		if (!m_TextureStorageReference->GetEditorTexture2DByName("MaterialAsset", IconTexture)) {
			if (!m_TextureStorageReference->GetEditorTexture2DByName("Error", IconTexture))
				return nullptr;
			return IconTexture;
		}

		return IconTexture;
	}break;
	default:
		if (m_TextureStorageReference->GetEditorTexture2DByName("Unknown", IconTexture))
			return IconTexture;
		else {
			if (!m_TextureStorageReference->GetEditorTexture2DByName("Error", IconTexture))
				return nullptr;
			return IconTexture;
		}
	}
}


void Editor::SetupContentBrowserStyle() {

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
}
void Editor::ClearContentBrowserStyle() {

	//Make sure its the same amount of PushStyleColor() calls in SetupContentBrowserStyle();
	ImGui::PopStyleColor(3);
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
void Editor::UpdateWindowPositions() {
	//NOTE: Seems like size then position cause a lot of positions depend on sizes
	//NOTE: Any hardcoded values here such as 100 200 etc means the value has not been implemented in code yet. (percentage values are fine)
	//NOTE: The percentage values can be changed safely and should be exposed later on
	//NOTE: A lot of these if not all could be calculated only when needed since they are required only at window creation

	m_DirectoryExplorerWindowSize = ImVec2(m_GUIViewport->Size.x * 0.1f, m_GUIViewport->Size.y * 0.3f);
	m_DetailsWindowSize = ImVec2(m_GUIViewport->Size.x * 0.2f, m_GUIViewport->Size.y - m_MainMenuBarSize.y);
	
	//lOOK AT CONTROL FLOW ESPECIALLY WITH HOW I SET THIS THEN. IT HAD A BUG BEFORE REGARDLESS
	if (!m_ContentBrowserOpened || m_ContentBrowserWindowSize.x == 0.0f && m_ContentBrowserWindowSize.y == 0.0f) //Second condition is for being true on first frame
		m_ContentBrowserWindowSize = ImVec2(m_GUIViewport->Size.x - m_DetailsWindowSize.x - m_DirectoryExplorerWindowSize.x, m_DirectoryExplorerWindowSize.y);

	m_NewContentWindowSize = ImVec2(m_GUIViewport->Size.x * 0.3f, m_GUIViewport->Size.y * 0.3f);
	m_HierarchyWindowSize = ImVec2(m_GUIViewport->Size.x * 0.1f, m_GUIViewport->Size.y - m_DirectoryExplorerWindowSize.y - m_MainMenuBarSize.y);

	if (!m_SpriteSelectorOpened)
		m_SpriteSelectorWindowSize = ImVec2(m_GUIViewport->Size.x * 0.2f, m_GUIViewport->Size.y * 0.6f);

	if (!m_MaterialEditorOpened)
		m_MaterialEditorWindowSize = ImVec2(m_GUIViewport->Size.x * 0.2f, m_GUIViewport->Size.y * 0.6f);

	m_ContentBrowserWindowPosition = ImVec2(m_DirectoryExplorerWindowSize.x, m_GUIViewport->Size.y - m_ContentBrowserWindowSize.y);
	m_DetailsWindowPosition = ImVec2(m_GUIViewport->Size.x - m_DetailsWindowSize.x, m_MainMenuBarSize.y);
	m_HierarchyWindowPosition = ImVec2(0.0f, m_MainMenuBarSize.y);
}