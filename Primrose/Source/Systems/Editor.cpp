#include "Systems/Editor.hpp"
#include "Systems/Core.hpp"



#include <iostream>


Editor::Editor(Core& core)
	: m_Core(&core)
{
	m_Window = m_Core->GetWindow();
	m_ECS = m_Core->GetECS();
	m_TextureStorage = m_Core->GetTextureStorage();
	m_AssetManager = m_Core->GetAssetManager();
	m_Input = m_Core->GetInput();
	m_Time = m_Core->GetTime();
	m_Serializer = m_Core->GetSerializer();
	m_Logger = m_Core->GetLogger();
	//Get folder texture?


	IMGUI_CHECKVERSION();
	m_ImGuiContext = ImGui::CreateContext();
	m_ImGuiViewport = ImGui::GetMainViewport();
	//TODO: Throw if any of these dont work!

	m_Logger->SystemLog("Dear IMGUI initialized.");
	std::string VersionMessage("Version: ");
	VersionMessage.append(ImGui::GetVersion());
	m_Logger->SystemLog(VersionMessage);


	m_ViewportCameraReference = &m_ECS->GetViewportCamera();
	if (m_ViewportCameraReference == nullptr)
		m_Logger->SystemLog("Failed to save viewport camera reference");


	m_IO = &ImGui::GetIO();
	m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// IF using Docking Branch
	m_IO->ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; //Disable IMGUI from interfering with glfw when it comes to cursor visibility and shape


	ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindowResource().m_Handle, true);
	ImGui_ImplOpenGL3_Init();


	m_SelectionWindows = std::make_unique<SelectionWindows>(core, *this);
	m_MaterialEditor = std::make_unique<MaterialEditor>(core, *this, *m_SelectionWindows); //Clean this
	m_DetailsWindow = std::make_unique<DetailsWindow>(core, *this, *m_SelectionWindows); //Clean this
	m_MainMenuBar = std::make_unique<MainMenuBar>(core, *this);
	m_ContentBrowser = std::make_unique<ContentBrowser>(core, *this);
	m_DebuggerWindow = std::make_unique<DebuggerWindow>(core, *this);
	m_SystemDebuggerWindow = std::make_unique<SystemDebuggerWindow>(core, *this);
	m_HierarchyWindow = std::make_unique<HierarchyWindow>(core, *this);
}
Editor::~Editor() {

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


[[nodiscard]] bool Editor::Update() {

	m_SelectionWindows->Update();
	
	UpdateViewportCameraInput();
	UpdateEditorInput();
	Render();

	return true;
}

void Editor::InitializeSubSystems() {
	//TODO: Unify the functions in all subsystems under one name. 
	//TODO: Setup and rework error messages in all init functions!
	m_MaterialEditor->Init();
	m_ContentBrowser->Init();
	m_DetailsWindow->Init();
	m_DebuggerWindow->Init();
	m_SystemDebuggerWindow->Init();
	m_HierarchyWindow->Init();

	m_SubSystemsInitialized = true;
}

void Editor::Render() {

	StartFrame();
	
	if (!m_SubSystemsInitialized) //Required since many Imgui functions cant be called before StartFrame()
		InitializeSubSystems();

	m_IsAnyWindowHovered = false; //Consider moving this to StartFrame() or something
	m_Logger->NewFrame();

	UpdateWindowPosition(); //NOTE: Currently updates sizes and positions
	m_EditorStyle.Apply();

	//Testing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//Physics* m_Physics = m_CoreReference->GetPhysics();
	//Vector3f Test 
	//	= m_Physics->ScreenToWorld(Vector2f(static_cast<float>(m_InputReference->CursorX), static_cast<float>(m_InputReference->CursorY)), 0.0f);

	//std::cout << "X: " << Test.m_X << " Y: " << Test.m_Y << " Z: " << Test.m_Z << std::endl;


	//Main Docking Space
	ImGuiDockNodeFlags Flags = 0;
	Flags |= ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(m_ImGuiViewport, Flags);


	//ImGui::ShowDemoWindow();
	//IMPORTANT NOTE: Make the size of each element in relation to the size of the viewport so they would scale with it
	//TODO: All the menus sizes and positions are relative to each other. Make sure to make the values used relative to each other instead of relying on literals
	m_DetailsWindow->Render();
	m_HierarchyWindow->Render();

	RenderAddGameObjectMenu();
	//RenderDirectoryExplorer();
	//RenderViewportWindow();
	m_MainMenuBar->Render();
	m_ContentBrowser->Render();
	m_DebuggerWindow->Render();
	m_SystemDebuggerWindow->Render();

	m_SelectionWindows->Render();
	m_MaterialEditor->Render();

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




void Editor::RenderViewportWindow() {

	//NOTE: I dont necesseraly need the viewport window to do the drag and drop check but i kept it for now incase of future use
	//NOTE: The tabs code could be done easily by getting the payload nad checking if no window is hovered.

	ImGuiWindowFlags ViewportFlags = 0;
	//ViewportFlags |= ImGuiWindowFlags_NoBackground;
	//ViewportFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	//ViewportFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
	ViewportFlags |= ImGuiWindowFlags_NoSavedSettings;
	//ViewportFlags |= ImGuiWindowFlags_NoMove;
	//ViewportFlags |= ImGuiWindowFlags_NoTitleBar;
	ViewportFlags |= ImGuiWindowFlags_NoDocking;
	bool Open = true; //? should i allow the viewport to be closable. Yes in the future.


	//Once
	static bool test = true;
	if (test) {
		ImGui::SetNextWindowSize(ImVec2(512, 512));
		ImGui::SetNextWindowPos(ImVec2(990, 540));
		test = false;
	}

	if (ImGui::Begin("ViewportWindow", &Open, ViewportFlags)) {



		m_ViewportSize = ImGui::GetWindowSize();
		ImVec2 WindowPosition = ImGui::GetWindowPos();
		std::cout << "X: " << WindowPosition.x << " Y: " << WindowPosition.y << std::endl;

		//NOTE: This will be put on ice until framebuffers are implemented!

		//m_CoreReference->GetWindow()->SetViewportSize(static_cast<int32>(m_ViewportSize.x), static_cast<int32>(m_ViewportSize.y));

		//Size seems fine but position seems reversed or something - Y seems the problem
		//m_CoreReference->GetWindow()->SetViewportPosition(static_cast<int32>(ImGui::GetWindowPos().x), -static_cast<int32>(ImGui::GetWindowPos().y));
	}

	ImGui::End();
}


void Editor::UpdateEditorInput() {

	//NOTE: Needs to be after the render calls otherwise its always false cause the flag is reseted right on top of this function

	//TODO: Deal with popups problem for ishovering check.

	//Unselect from hieracrchy
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && m_SelectedGameObject != nullptr && !m_IsAnyWindowHovered) {
		//The only problem with this is that the IsAnyWindowHovered will not catch windows if a popup is open
		SetSelectedGameObject(nullptr);
	}

	if (m_ContentBrowser->IsContentBrowserWindowHovered() && !ImGui::IsAnyItemHovered()){

		if (m_SelectedContentElement != nullptr && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			m_SelectedContentElement = nullptr;

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
			m_Core->ErrorLog("Released");
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
void Editor::UpdateViewportCameraInput() {

	if (m_IsAnyWindowHovered)
		return;


	//NOTE: This function could be broken into 3 or 4 parts.

	if (m_Input->GetMouseKey(MouseKeyCode::RIGHT) && !m_ViewportNavigationMode) {
		m_ViewportNavigationMode = true;
		m_Input->SetMouseInputMode(MouseMode::DISABLED);
	}
	else if (!m_Input->GetMouseKey(MouseKeyCode::RIGHT) && m_ViewportNavigationMode) {
		m_ViewportNavigationMode = false;
		m_Input->SetMouseInputMode(MouseMode::NORMAL);
	}

	if (!m_ViewportNavigationMode) {
		return;
	}

	if (m_ViewportCameraReference == nullptr)
		return;

	const float DeltaTime = static_cast<float>(m_Time->GetDeltaTime());


	if (m_Input->GetKey(Keycode::W)) {
		m_ViewportCameraReference->MoveY(m_CameraMovementSpeed * DeltaTime);
	}
	if (m_Input->GetKey(Keycode::S)) {
		m_ViewportCameraReference->MoveY((m_CameraMovementSpeed * DeltaTime) * -1);
	}
	if (m_Input->GetKey(Keycode::A)) {
		m_ViewportCameraReference->MoveX((m_CameraMovementSpeed * DeltaTime) * -1);
	}
	if (m_Input->GetKey(Keycode::D)) {
		m_ViewportCameraReference->MoveX(m_CameraMovementSpeed * DeltaTime);
	}
	if (m_Input->GetKey(Keycode::E)) {
		m_ViewportCameraReference->MoveVertical(m_CameraMovementSpeed * DeltaTime);
	}
	if (m_Input->GetKey(Keycode::Q)) {
		m_ViewportCameraReference->MoveVertical((m_CameraMovementSpeed * DeltaTime) * -1);
	}

	const Vector2f CursorDelta = m_Input->GetMouseCursorDelta();

	//TODO: Make funcs for those
	if (CursorDelta.m_X >= m_FreeLookSensitivity)
		m_ViewportCameraReference->RotateY(m_FreeLookSpeed * DeltaTime * CursorDelta.m_X);
	if (CursorDelta.m_X <= -m_FreeLookSensitivity)
		m_ViewportCameraReference->RotateY((m_FreeLookSpeed * DeltaTime * -CursorDelta.m_X) * -1);

	if (CursorDelta.m_Y >= m_FreeLookSensitivity)
		m_ViewportCameraReference->RotateX(m_FreeLookSpeed * DeltaTime * CursorDelta.m_Y);
	if (CursorDelta.m_Y <= -m_FreeLookSensitivity)
		m_ViewportCameraReference->RotateX((m_FreeLookSpeed * DeltaTime * -CursorDelta.m_Y) * -1);

	const float ScrollDelta = m_Input->GetScrollDelta();

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





void Editor::OpenAsset(Asset& asset) {

	//Add more types here!
	if (asset.m_Type == AssetType::MATERIAL) {
		m_MaterialEditor->SetWindowState(true);
		m_MaterialEditor->SetTarget(m_AssetManager->GetMaterial(asset));
	}



}


void Editor::AddSpacings(uint32 count) {
	for (uint32 index = 0; index < count; index++)
		ImGui::Spacing();
}
void Editor::AddSeparators(uint32 count) {
	for (uint32 index = 0; index < count; index++)
		ImGui::Separator();
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
			if (!m_TextureStorage->GetEditorTexture2DByName(asset.m_Name, IconTexture)) {
				if (!m_TextureStorage->GetEditorTexture2DByName("Error.png", IconTexture))
					return nullptr;
				return IconTexture;
			}
		}
		else {
			if (!m_TextureStorage->GetTexture2DByName(asset.m_Name, IconTexture)) {
				if (!m_TextureStorage->GetEditorTexture2DByName("Error.png", IconTexture))
					return nullptr;
				return IconTexture;
			}
		}

		return IconTexture;
	}break;
	case AssetType::MATERIAL: {

		if (!m_TextureStorage->GetEditorTexture2DByName("MaterialAsset.png", IconTexture)) {
			if (!m_TextureStorage->GetEditorTexture2DByName("Error.png", IconTexture))
				return nullptr;
			return IconTexture;
		}

		return IconTexture;
	}break;
	default:
		if (m_TextureStorage->GetEditorTexture2DByName("Unknown.png", IconTexture))
			return IconTexture;
		else {
			if (!m_TextureStorage->GetEditorTexture2DByName("Error.png", IconTexture))
				return nullptr;
			return IconTexture;
		}
	}
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
void Editor::UpdateWindowPosition() {
	//NOTE: Seems like size then position cause a lot of positions depend on sizes
	//NOTE: Any hardcoded values here such as 100 200 etc means the value has not been implemented in code yet. (percentage values are fine)
	//NOTE: The percentage values can be changed safely and should be exposed later on
	//NOTE: A lot of these if not all could be calculated only when needed since they are required only at window creation

	//m_DirectoryExplorerWindowSize = ImVec2(m_GUIViewport->Size.x * 0.1f, m_GUIViewport->Size.y * 0.3f);
	//m_DetailsWindowSize = ImVec2(m_GUIViewport->Size.x * 0.2f, m_GUIViewport->Size.y - m_Size.y);
	
	//lOOK AT CONTROL FLOW ESPECIALLY WITH HOW I SET THIS THEN. IT HAD A BUG BEFORE REGARDLESS
	//if (!m_ContentBrowserOpened || m_ContentBrowserWindowSize.x == 0.0f && m_ContentBrowserWindowSize.y == 0.0f) //Second condition is for being true on first frame
		//m_ContentBrowserWindowSize = ImVec2(m_GUIViewport->Size.x - m_DetailsWindowSize.x - m_DirectoryExplorerWindowSize.x, m_DirectoryExplorerWindowSize.y);


	//NOTE: By the end of the cleanup, this function should be gone.

	//Probably should be here with getters for the other classes to use!
	m_NewStandaloneWindowSize = ImVec2(m_ImGuiViewport->Size.x * 0.3f, m_ImGuiViewport->Size.y * 0.3f);






	//Consider making this vars maybe?


	//m_ContentBrowserWindowPosition = ImVec2(m_DirectoryExplorerWindowSize.x, m_GUIViewport->Size.y - m_ContentBrowserWindowSize.y);
	//m_DetailsWindowPosition = ImVec2(m_GUIViewport->Size.x - m_DetailsWindowSize.x, m_Size.y);

}