#include "Systems/Editor.hpp"
#include "Systems/Core.hpp"



#include <iostream>


Editor::Editor(Core& core)
	: m_Core(&core)
{
	m_Window = core.GetWindow();
	m_ECS = core.GetECS();
	m_TextureStorage = core.GetTextureStorage();
	m_AssetManager = core.GetAssetManager();
	m_Input = core.GetInput();
	m_Time = core.GetTime();
	m_Serializer = core.GetSerializer();
	m_Logger = core.GetLogger();
	//Get folder texture?

	//TODO: Break this into functions! Initialize DearIngui etc

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


	m_SelectionWindows = std::make_unique<SelectionWindow>(core, *this);
	m_MaterialEditor = std::make_unique<MaterialEditor>(core, *this);
	m_DetailsWindow = std::make_unique<DetailsWindow>(core, *this);
	m_MainMenuBar = std::make_unique<MainMenuBar>(core, *this);
	m_ContentBrowser = std::make_unique<ContentBrowser>(core, *this);
	m_DebugLogWindow = std::make_unique<DebugLogWindow>(core, *this);
	m_SystemLogWindow = std::make_unique<SystemLogWindow>(core, *this);
	m_HierarchyWindow = std::make_unique<HierarchyWindow>(core, *this);
}
Editor::~Editor() {

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


[[nodiscard]] bool Editor::Update() {

	UpdateViewportCameraInput();
	Render();
	return true;
}

bool Editor::InitializeSubSystems() {

	//TODO: Implement proper error handling for this function and all the Init()s

	if (m_SubSystemsInitialized) //Required since many Imgui functions cant be called before StartFrame()
		return false;

	//TODO: Setup and rework error messages in all init functions!


	//IMPORTANT NOTE: I WAS TESTING THE BUFFERSIZE FOR NAME AND TAG BY NOT ADDING -1 TO WHEN ITS USED!!!!!!
	//NOTE: Either keep this or make it so that it will do this in Update depending on a bool.


	m_MainMenuBar->Init();

	m_MaterialEditor->Init();
	m_ContentBrowser->Init();
	m_DetailsWindow->Init();
	m_DebugLogWindow->Init();
	m_SystemLogWindow->Init();
	m_HierarchyWindow->Init();
	m_SelectionWindows->Init();

	m_SubSystemsInitialized = true;

	return true;
}
void Editor::UpdateSubsystems() {


	//Rename folder and these to modules?
	m_MaterialEditor->Update();
	m_SelectionWindows->Update(); //Add this for all since this is where i check input! and sometimes do other stuff!
	m_ContentBrowser->Update();
	m_DetailsWindow->Update();

	//The input 
	//NOTE: Needs to be after the render calls otherwise its always false cause the flag is reseted right on top of this function
	//TODO: Deal with popups problem for ishovering check.
	m_ContentBrowser->Update();
	m_HierarchyWindow->Update();
}

void Editor::Render() {

	StartFrame();
	//I should probably just do it here.

	m_NewStandaloneWindowSize = ImVec2(m_ImGuiViewport->Size.x * 0.3f, m_ImGuiViewport->Size.y * 0.3f); //Move somewhere else and it needs to be after startframe! Maybe just move this line 
	//to the func itself and get rid of the variable.
	UpdateSubsystems(); //Requires a call to StartFrame()

	m_IsAnyWindowHovered = false; //Consider moving this to StartFrame() or something


	m_EditorStyle.Apply();



	//Testing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//Physics* m_Physics = m_CoreReference->GetPhysics();
	//Vector3f Test 
	//	= m_Physics->ScreenToWorld(Vector2f(static_cast<float>(m_InputReference->CursorX), static_cast<float>(m_InputReference->CursorY)), 0.0f);

	//std::cout << "X: " << Test.m_X << " Y: " << Test.m_Y << " Z: " << Test.m_Z << std::endl;


	//FINAL NOTES REGARDING WINDOWS SIZING/POSITIONING PROBLEM
	//-There is going to be an implicit order here when it comes to rendering. I could move the order to Update() tho! no you usually assum an implicit order dependency
	//in render functions either way.
	//I could have abstracted the sizes to their results in the getters instead of returning the member variable. However, this means calls to the getters before Init()
	//-is called will crash or i would return 0,0. So the implicit order dependency is to be kept.


	//It would be MainMenuBar->DetailsWindow then either ContentBrowser or HierarchyWindow. Depending which one decides the height of the other.
	//Probably keep a Clockwise pattern to it.


	//Main Docking Space
	ImGuiDockNodeFlags Flags = 0;
	Flags |= ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport(m_ImGuiViewport, Flags);


	//ImGui::ShowDemoWindow();
	m_MainMenuBar->Render();
	m_DetailsWindow->Render();
	m_ContentBrowser->Render();
	m_HierarchyWindow->Render();

	//NOTE: There is a couple of connections that are not finished yet.
	//Directory Explorer and Content Browser are not fully linked during resizing
	//Hierarchy is not linked to Content Browser.
	//The add button and the current size of the hierarchy window.
	//Resize windows and try to find all user cases.


	//RenderViewportWindow();
	m_DebugLogWindow->Render();
	m_SystemLogWindow->Render();
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

void Editor::CheckForHoveredWindows() {
	if (ImGui::IsWindowHovered())
		m_IsAnyWindowHovered = true;
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