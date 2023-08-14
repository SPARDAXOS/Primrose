#include "Systems/Core.hpp"
#include <iostream>



Core::Core() noexcept {

	//Order matters
	m_Time = std::make_unique<Time>();
	m_Logger = std::make_unique<Logger>(*m_Time);

	m_Window = std::make_unique<Window>(*this);
	m_TextureStorage = std::make_unique<TextureStorage>(*this);
	m_Serializer = std::make_unique<Serializer>(*this);
	m_ECS = std::make_unique<EntityComponentSystem>(*this);
	m_AssetManager = std::make_unique<AssetManager>(*this);
	m_Input = std::make_unique<Input>(*m_Window);
	m_Editor = std::make_unique<Editor>(*this);
	m_Renderer = std::make_unique<Renderer>(*this);
	m_Physics = std::make_unique<Physics>(*this);
}
void Core::SetupCore() { // Sounds like 2 step initialization.

	m_Window->SetWindowMode(WindowMode::WINDOWED);
	m_Renderer->CheckRendererAPIVersion();
	m_AssetManager->LoadAssets();
	m_Editor->SaveEngineTexturesReferences();



	SystemLog("Primrose initialized successfully.");
}


void Core::Run() {
	//Run all operations before simply updating the systems in a loop
	m_Running = true;


	SetupCore();
	//Textures
	//TODO: Implement own image loader! for bitmaps at least own decoder




	Texture2D* CrateTexture = nullptr; 
	if (!m_TextureStorage->GetTexture2DByName("Crate", CrateTexture))
		PrintMessage("It failed to load the texture!");

	
	//CreateTexture->Bind();
	
	//New Planned Features
	//Materials - Some default ones like lit, unlit
	//Use ShaderCompiler again to load shaders using specific params
	//Using the params, the parser will parse specific parts of shaders by shaking for markers ###Bloom Skip: X
	//It will load parts of the shader depending on the params
	//It will save it so next time something wants a shader with these params, it will look if they are saved and return that same shader
	
	//Renderer takes a material

	
	//ECS
	GameObject* GameObjectTest = &m_ECS->CreateGameObject("GameObject1");
	GameObjectTest->AddComponent<SpriteRenderer>();
	SpriteRenderer* Component = GameObjectTest->GetComponent<SpriteRenderer>();
	Component->SetSprite(CrateTexture);

	//IMPORTANT NOTE: This is temporary so this will leak memory in the meanwhile
	Asset* TestAsset = new Asset();
	Material* TestMaterial = new Material(*TestAsset, *m_AssetManager);
	Component->SetMaterial(TestMaterial);

	Texture2D* AmbientTexture = nullptr;
	if (m_TextureStorage->GetTexture2DByName("Crate2", AmbientTexture))
		Component->GetMaterial()->m_Ambient = AmbientTexture;

	Texture2D* SpecularTexture = nullptr;
	if (m_TextureStorage->GetTexture2DByName("CrateSpecular", SpecularTexture))
		Component->GetMaterial()->m_Specular = SpecularTexture;


	Component->GetMaterial()->m_AmbientStrength = 0.4f;
	Component->GetMaterial()->m_SpecularStrength = 0.9f;
	Component->GetMaterial()->m_SpecularShininess = 16;



	GameObject* GameObjectTestChild = &m_ECS->CreateGameObject("GameObject2");
	GameObjectTestChild->AddComponent<SpriteRenderer>();
	SpriteRenderer* ComponentChild = GameObjectTestChild->GetComponent<SpriteRenderer>();
	ComponentChild->SetSprite(CrateTexture);

	GameObject* GameObjectTestChildChild = &m_ECS->CreateGameObject("GameObject3");
	GameObjectTestChildChild->AddComponent<SpriteRenderer>();
	SpriteRenderer* ComponentChildChild = GameObjectTestChildChild->GetComponent<SpriteRenderer>();
	ComponentChildChild->SetSprite(CrateTexture);


	GameObject* GameObjectTest2 = &m_ECS->CreateGameObject("GameObject4");
	GameObjectTest2->AddComponent<SpriteRenderer>();
	SpriteRenderer* Component2 = GameObjectTest2->GetComponent<SpriteRenderer>();
	Component2->SetSprite(CrateTexture);

	GameObject* GameObjectTestChild2 = &m_ECS->CreateGameObject("GameObject5");
	GameObjectTestChild2->AddComponent<SpriteRenderer>();
	SpriteRenderer* ComponentChild2 = GameObjectTestChild2->GetComponent<SpriteRenderer>();
	ComponentChild2->SetSprite(CrateTexture);

	GameObject* GameObjectTestChildChild2 = &m_ECS->CreateGameObject("GameObject6");
	GameObjectTestChildChild2->AddComponent<SpriteRenderer>();
	SpriteRenderer* ComponentChildChild2 = GameObjectTestChildChild2->GetComponent<SpriteRenderer>();
	ComponentChildChild2->SetSprite(CrateTexture);

	//TODO: It is important to decide what to do when it comes to multiple components of the same type and such

	GameObjectTest->AddChild(GameObjectTestChild); //Add
	GameObjectTestChild->AddChild(GameObjectTestChildChild); //Multiple Steps Deep

	GameObjectTest2->AddChild(GameObjectTestChild2); //Add
	GameObjectTestChild2->AddChild(GameObjectTestChildChild2); //Multiple Steps Deep

	//Component->SetTint(Colors::Red); //TODO: Remove vertex color attribute + clean up shaders + readjust attributes afterwards

	Transform* GameObjectTransform = &GameObjectTest->GetTransform();
	GameObjectTransform->m_Position = Vector3f(5.0f, 0.0f, 0.0f);
	GameObjectTransform->m_Rotation = Vector3f(0.0f, 0.0f, 0.0f);
	GameObjectTransform->m_Scale = Vector3f(1.0f, 1.0f, 1.0f);


	Transform* GameObjectTransform2 = &GameObjectTest2->GetTransform();
	GameObjectTransform2->m_Position = Vector3f(-5.0f, 0.0f, 0.0f);
	GameObjectTransform2->m_Rotation = Vector3f(0.0f, 0.0f, 0.0f);
	GameObjectTransform2->m_Scale = Vector3f(1.5f, 1.5f, 1.5f);
	

	
	while (m_Running) {

		if (m_Input->GetKey(Keycode::R))
			GameObjectTransform->m_Rotation.m_Y += 1.0f;

		UpdateSystems();
	}
	
	Exit();
	//End all operations then clean up
}
void Core::Exit() {

	PrintExitMessage();
}
void Core::UpdateSystems() {


	//TODO: Create a pool of threads and manage it by camsizing the allowed threads count depending on the system
	//Do stuff like joinable checkls and whatever

	//BUG: The aspect ratio maybe should be synced with the size of the screen?

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


	m_Window->ClearBuffer();


	if (!m_Renderer->Update()) {
		RegisterExitMessage("Engine closed down.\nReason: " + m_Renderer->GetLastExitMessage());
		m_Running = false;
	}
	if (!m_Editor->Update()) {
		//TODO: Implement error handling in Editor class
		m_Running = false;
	}

	//Needs to be manual thing to call from window - Remove from Renderer - Maybe add this and clear to window
	m_Window->SwapBuffer();

	//glfwPollEvents(); //Needs to be outside of the worker thread calls
}


void Core::DebugLog(std::string message) noexcept {
	m_Logger->DebugLog(message);
}
void Core::WarningLog(std::string message) noexcept {
	m_Logger->WarningLog(message);
}
void Core::ErrorLog(std::string message) noexcept {
	m_Logger->ErrorLog(message);
}
void Core::SystemLog(std::string message) noexcept {
	m_Logger->SystemLog(message);
}

void Core::RegisterExitMessage(std::string message) noexcept {
	m_ExitMessage = message;
}
void Core::PrintExitMessage() noexcept {
	printf(m_ExitMessage.data());
}