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
void Core::SetupCore() { // Sounds like 2 step initialization. I could however use it for stuff like cursor pos first capture.

	glfwGetCursorPos(m_Window.get()->GetWindowResource().m_ptr, &m_LastCursorPositionX, &m_LastCursorPositionY);
}


void Core::Run() {
	//Run all operations before simply updating the systems in a loop
	m_Running = true;


	//Textures
	//TODO: Implement own image loader! for bitmaps at least own decoder
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
}
void Core::UpdateSystems() {

	if (!m_Window->Update()) {
		RegisterExitMessage("Engine closed down.\nReason: " + m_Window->GetLastExitMessage());
		m_Running = false; //TODO: make cleaner exit method
	}

	if (!m_Renderer->Update()) {
		RegisterExitMessage("Engine closed down.\nReason: " + m_Renderer->GetLastExitMessage());
		m_Running = false;
	}

	m_Time->Update();

	UpdateViewportControls();

}
void Core::UpdateViewportControls() noexcept {

	Camera* ViewportCamera = &m_ECS->GetViewportCamera();
	if (ViewportCamera == nullptr)
		return;

	GLFWwindow* WindowReference = m_Window->GetWindowResource().m_ptr;
	const float DeltaTime = static_cast<float>(m_Time->GetDeltaTime());

	std::cout << "X: " << ViewportCamera->GetOwner()->GetTransform().m_Rotation.m_X
		<< " " << "Y: " << ViewportCamera->GetOwner()->GetTransform().m_Rotation.m_Y
		<< " " << "Z: " << ViewportCamera->GetOwner()->GetTransform().m_Rotation.m_Z << std::endl;

	
	if (glfwGetKey(WindowReference, GLFW_KEY_W)){
		ViewportCamera->MoveY(m_CameraMovementSpeed * DeltaTime);
	}
	if (glfwGetKey(WindowReference, GLFW_KEY_S)) {
		ViewportCamera->MoveY((m_CameraMovementSpeed * DeltaTime) * -1);
	}
	if (glfwGetKey(WindowReference, GLFW_KEY_A)) {
		ViewportCamera->MoveX((m_CameraMovementSpeed * DeltaTime) * -1);
	}
	if (glfwGetKey(WindowReference, GLFW_KEY_D)) {
		ViewportCamera->MoveX(m_CameraMovementSpeed * DeltaTime);
	}
	if (glfwGetKey(WindowReference, GLFW_KEY_E)) {
		ViewportCamera->MoveVertical(m_CameraMovementSpeed * DeltaTime);
	}
	if (glfwGetKey(WindowReference, GLFW_KEY_Q)) {
		ViewportCamera->MoveVertical((m_CameraMovementSpeed * DeltaTime) * -1);
	}


	//Look
	double X;
	double Y;
	glfwGetCursorPos(WindowReference, &X, &Y);

	const float XResults = static_cast<float>(X - m_LastCursorPositionX);
	const float YResults = static_cast<float>(m_LastCursorPositionY - Y); //Reversed cause Up is minus, Down is plus.

	//Make funcs for those
	if (XResults >= m_FreeLookSensitivity)
		ViewportCamera->RotateY(m_FreeLookSpeed * DeltaTime * XResults);
	else if (XResults <= -m_FreeLookSensitivity)
		ViewportCamera->RotateY((m_FreeLookSpeed * DeltaTime * -XResults) * -1);

	if (YResults >= m_FreeLookSensitivity)
		ViewportCamera->RotateX(m_FreeLookSpeed * DeltaTime * YResults);
	else if (YResults <= -m_FreeLookSensitivity)
		ViewportCamera->RotateX((m_FreeLookSpeed * DeltaTime * -YResults) * -1);

	m_LastCursorPositionX = X;
	m_LastCursorPositionY = Y;


	//Scroll Wheel
	if (ScrollCapture::ScrollY > 0.0f) {
		m_CameraMovementSpeed += m_CameraSpeedIncrease * DeltaTime;
		if (m_CameraMovementSpeed > m_CameraSpeedMax)
			m_CameraMovementSpeed = m_CameraSpeedMax;
		ScrollCapture::ScrollY = 0; //Reset it for next frame
	}
	else if (ScrollCapture::ScrollY < 0.0f) {
		m_CameraMovementSpeed -= m_CameraSpeedDecrease * DeltaTime;
		if (m_CameraMovementSpeed < m_CameraSpeedMin)
			m_CameraMovementSpeed = m_CameraSpeedMin;
		ScrollCapture::ScrollY = 0; //Reset it for next frame
	}
}

void Core::RegisterExitMessage(std::string message) noexcept {
	m_ExitMessage = message;
}
void Core::PrintExitMessage() noexcept {
	printf(m_ExitMessage.data());
}