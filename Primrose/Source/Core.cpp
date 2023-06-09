#include <Core.hpp>
#include <iostream>



Core::Core() noexcept {

	//TODO: is error checking needed here? probably not since they throw and i should just let it bubble up.

	m_TextureStorage = std::make_unique<TextureStorage>();
	m_ECS = std::make_unique<EntityComponentSystem>();
	m_Window = std::make_unique<Window>(m_ViewportWidth, m_ViewportHeight);
	m_Renderer = std::make_unique<Renderer>(*m_ECS.get(), *m_Window.get());
	m_Time = std::make_unique<Time>();
	m_Input = std::make_unique<Inputinator>(*m_Window.get()->GetWindowResource().m_ptr);
}
void Core::SetupCore() { // Sounds like 2 step initialization.

}


void Core::Run() {
	//Run all operations before simply updating the systems in a loop
	m_Running = true;


	//Textures
	//TODO: Implement own image loader! for bitmaps at least own decoder

	//TODO: Maybe switch this up so it returns a texture pointer. The actual texture is stored in the storage so if you attempt to load it again, it will return the 
	//one from the storage. It checks by path and not name! or?
	const std::string_view TexturePath = "Resources/Textures/Crate.jpg";
	const std::string_view TextureName = "Create";
	Texture2D* CrateTexture;
	
	if (!m_TextureStorage->LoadTexture2D(TexturePath, TextureName, CrateTexture))
	PrintMessage("It failed to load the texture!");
	
	//CreateTexture->Bind();
	m_TextureStorage->ActivateTextureUnit(GL_TEXTURE0);
	
	//New Planned Features
	//Materials - Some default ones like lit, unlit
	//Use ShaderCompiler again to load shaders using specific params
	//Using the params, the parser will parse specific parts of shaders by shaking for markers ###Bloom Skip: X
	//It will load parts of the shader depending on the params
	//It will save it so next time something wants a shader with these params, it will look if they are saved and return that same shader
	
	//Renderer takes a material



	//For magnifying and minifying textures - When texture is smaller than object or bigger than object!
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)); // For mipmaps. Cuase gets smaller while the magnifying doesnt use mipmaps so dont use it!
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	
	//GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, CreateTexture->GetWidth(), CreateTexture->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, CreateTexture->GetData()));
	//Image data might not be needed after this!
	//GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	
	//CreateTexture->Unbind();
	
	//Should be when shader program is created. ? idk anymore
	//ShaderProgramTest.SetUniform("uDiffuse", TextureUnit::DIFFUSE);


	//ImGuiContext* GUIContext = ImGui::CreateContext();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// IF using Docking Branch
	
	ImGui_ImplGlfw_InitForOpenGL(m_Window->GetWindowResource().m_ptr, true);
	ImGui_ImplOpenGL3_Init();
	
	//ECS
	GameObject* GameObjectTest = &m_ECS->CreateGameObject("Test");
	GameObjectTest->AddComponent<SpriteRenderer>();
	SpriteRenderer* Component = GameObjectTest->GetComponent<SpriteRenderer>();
	Component->SetSprite(CrateTexture);
	//Component->SetTint(Colors::Red); //TODO: Remove vertex color attribute + clean up shaders + readjust attributes afterwards

	//GameObjectTest->HasComponent<SpriteRenderer>();
	//GameObjectTest->RemoveComponent<SpriteRenderer>();
	//GameObjectTest->HasComponent<SpriteRenderer>();
	//GameObjectTest->AddComponent<SpriteRenderer>();
	//GameObjectTest->HasComponent<SpriteRenderer>();
	Transform* GameObjectTransform = &GameObjectTest->GetTransform();
	GameObjectTransform->m_Position = Vector3f(-0.6f, 0.2f, 0.0f);
	GameObjectTransform->m_Rotation = Vector3f(0.0f, 0.0f, 0.0f);
	GameObjectTransform->m_Scale = Vector3f(0.5f, 0.5f, 0.5f);
	
	
	//GameObject* InstansiatedGameObject = &m_ECS->Instantiate(*GameObjectTest);
	//SpriteRenderer* NewSpriteRenderer 
		//= InstansiatedGameObject->AddComponent<SpriteRenderer>(); //Will not work cause the flags carried over but not actual components.
	//NewSpriteRenderer->SetSprite(CreateTexture);
	
	//InstansiatedGameObject->GetTransform().m_Position.m_X *= -1;
	SetupCore();
	
	while (m_Running) {

		UpdateSystems();
	}
	
	Exit();
	//End all operations then clean up
}
void Core::Exit() {

	PrintExitMessage();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
void Core::UpdateSystems() {


	//TODO: Create a pool of threads and manage it by camsizing the allowed threads count depending on the system
	//Do stuff like joinable checkls and whatever

	m_Time->Update();

	//PollEvents
	if (!m_Window->Update()) {
		RegisterExitMessage("Engine closed down.\nReason: " + m_Window->GetLastExitMessage());
		m_Running = false; //TODO: make cleaner exit method
	}
	if (!m_Input->Update()) {
		RegisterExitMessage("Engine closed down.\nReason: " + m_Input->GetLastExitMessage());
		m_Running = false;
	}
	if (!m_ECS->Update()) {
		RegisterExitMessage("Engine closed down.\nReason: " + m_ECS->GetLastExitMessage());
		m_Running = false;
	}


	//Feed input and start GUI frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();

	//Render GUI
	ImGui::Begin("Demo window");
	ImGui::Button("Hello!");
	ImGui::End();

	if (!m_Renderer->Update()) {
		RegisterExitMessage("Engine closed down.\nReason: " + m_Renderer->GetLastExitMessage());
		m_Running = false;
	}

	//Render GUI to screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_Window->GetWindowResource().m_ptr);


	UpdateViewportControls();


	//glfwPollEvents(); //Needs to be outside of the worker thread calls
}
void Core::UpdateViewportControls() {
	
	if (m_Input->GetMouseKey(MouseKeyCode::RIGHT) && !m_ViewportNavigationMode) {
		m_ViewportNavigationMode = true;
		m_Input->SetMouseInputMode(MouseMode::HIDDEN);
	}
	else if (!m_Input->GetMouseKey(MouseKeyCode::RIGHT) && m_ViewportNavigationMode) {
		m_ViewportNavigationMode = false;
		m_Input->SetMouseInputMode(MouseMode::NORMAL);
	}
	
	if (!m_ViewportNavigationMode) {
		return;
	}

	Camera* ViewportCamera = &m_ECS->GetViewportCamera();
	if (ViewportCamera == nullptr)
		return;

	const float DeltaTime = static_cast<float>(m_Time->GetDeltaTime());

	
	if (m_Input->GetKey(Keycode::W)){
		ViewportCamera->MoveY(m_CameraMovementSpeed * DeltaTime);
	}
	if (m_Input->GetKey(Keycode::S)) {
		ViewportCamera->MoveY((m_CameraMovementSpeed * DeltaTime) * -1);
	}
	if (m_Input->GetKey(Keycode::A)) {
		ViewportCamera->MoveX((m_CameraMovementSpeed * DeltaTime) * -1);
	}
	if (m_Input->GetKey(Keycode::D)) {
		ViewportCamera->MoveX(m_CameraMovementSpeed * DeltaTime);
	}
	if (m_Input->GetKey(Keycode::E)) {
		ViewportCamera->MoveVertical(m_CameraMovementSpeed * DeltaTime);
	}
	if (m_Input->GetKey(Keycode::Q)) {
		ViewportCamera->MoveVertical((m_CameraMovementSpeed * DeltaTime) * -1);
	}

	const Vector2f CursorDelta = m_Input->GetMouseCursorDelta();

	//TODO: Moving the camera and also looking in one direction will break it after a while
	//Make funcs for those
	if (CursorDelta.m_X >= m_FreeLookSensitivity)
		ViewportCamera->RotateY(m_FreeLookSpeed * DeltaTime * CursorDelta.m_X);
	if (CursorDelta.m_X <= -m_FreeLookSensitivity)
		ViewportCamera->RotateY((m_FreeLookSpeed * DeltaTime * -CursorDelta.m_X) * -1);

	if (CursorDelta.m_Y >= m_FreeLookSensitivity)
		ViewportCamera->RotateX(m_FreeLookSpeed * DeltaTime * CursorDelta.m_Y);
	if (CursorDelta.m_Y <= -m_FreeLookSensitivity)
		ViewportCamera->RotateX((m_FreeLookSpeed * DeltaTime * -CursorDelta.m_Y) * -1);

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

void Core::RegisterExitMessage(std::string message) noexcept {
	m_ExitMessage = message;
}
void Core::PrintExitMessage() noexcept {
	printf(m_ExitMessage.data());
}