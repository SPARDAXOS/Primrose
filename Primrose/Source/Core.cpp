#include <Core.hpp>
#include <iostream>



Core::Core() noexcept {

	//TODO: is error checking needed here? probably not since they throw and i should just let it bubble up.

	m_TextureStorage = std::make_unique<TextureStorage>();
	m_ECS = std::make_unique<EntityComponentSystem>();
	m_Window = std::make_unique<Window>();
	m_Renderer = std::make_unique<Renderer>(*m_ECS.get(), *m_Window.get());
	m_Time = std::make_unique<Time>();
}


void Core::SetupCore() { // Sounds like 2 step initialization

}


void Core::Run() {
	//Run all operations before simply updating the systems in a loop
	m_Running = true;


	//Shaders
	//Shader VertexShader(GL_VERTEX_SHADER, "Resources/Shaders/Vertex.glsl");
	//Shader FragmentShader(GL_FRAGMENT_SHADER, "Resources/Shaders/Fragment.glsl");

	//ShaderProgram ShaderProgramTest;
	//ShaderProgramTest.AttachShader(VertexShader);
	//ShaderProgramTest.AttachShader(FragmentShader);
	//if (!ShaderProgramTest.LinkShaderProgram())
	//	PrintMessage("LinkShaderProgram Failed");
	//ShaderProgramTest.Bind();

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



	//TODO: when it comes to a HasComponenet() function for the component interface. Every time a component is added or removed, it checks a bit flag that is in the game object.
	//So HasComponent() simply checks the bit flag whether its 1 or 0 to check if a gameobject has a specific componenet.

	
	//ECS
	GameObject* GameObjectTest = &m_ECS->CreateGameObject("Test");
	GameObjectTest->AddComponent<SpriteRenderer>();
	SpriteRenderer* Component = GameObjectTest->GetComponent<SpriteRenderer>();
	Component->SetSprite(CrateTexture);
	Component->SetTint(Colors::Red); //TODO: Remove vertex color attribute + clean up shaders + readjust attributes afterwards


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


	while (m_Running) {
		std::cout << "FPS: " << m_Time->GetFPS() << std::endl;
		
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

}


void Core::RegisterExitMessage(std::string message) noexcept {
	m_ExitMessage = message;
}
void Core::PrintExitMessage() noexcept {
	printf(m_ExitMessage.data());
}