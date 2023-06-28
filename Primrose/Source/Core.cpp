#include <Core.hpp>
#include <iostream>


Core::Core() {

	//TODO: is error checking needed here? probably not since they throw and i should just let it bubble up.

	m_TextureStorage = std::make_unique<TextureStorage>();
	m_ECS = std::make_unique<EntityComponentSystem>();
	m_Window = std::make_unique<Window>();
	m_Renderer = std::make_unique<Renderer>(*m_ECS.get(), *m_Window.get());
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
	const Texture2D* CreateTexture;

	if (!m_TextureStorage->LoadTexture2D(TexturePath, TextureName, CreateTexture))
		PrintMessage("It failed to load the texture!");

	CreateTexture->Bind();
	m_TextureStorage->ActivateTextureUnit(GL_TEXTURE0);
	m_TextureStorage->SetSamplerState(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	m_TextureStorage->SetSamplerState(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	m_TextureStorage->SetSamplerState(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_TextureStorage->SetSamplerState(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//For magnifying and minifying textures - When texture is smaller than object or bigger than object!
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)); // For mipmaps. Cuase gets smaller while the magnifying doesnt use mipmaps so dont use it!
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, CreateTexture->GetWidth(), CreateTexture->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, CreateTexture->GetData()));
	//Image data might not be needed after this!
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));

	CreateTexture->Unbind();

	//ShaderProgramTest.SetUniform("uDiffuse", TextureUnit::DIFFUSE);


	//Creat matrix out of the Translation, Rotation and Scale vectors using this 

	//TODO: when it comes to a HasComponenet() function for the component interface. Every time a component is added or removed, it checks a bit flag that is in the game object.
	//So HasComponent() simply checks the bit flag whether its 1 or 0 to check if a gameobject has a specific componenet.

	//Transformations
	//Transform TransformTest;
	//TransformTest.m_Position = Vector3f(-0.6f, 0.2f, 0.0f);
	//TransformTest.m_Rotation = Vector3f(0.0f, 0.0f, 90.0f);
	//TransformTest.m_Scale    = Vector3f(0.5f, 0.5f, 0.5f);
	//TransformTest.UpdateMatrix();

	//ECS
	GameObject* GameObjectTest = &m_ECS->CreateGameObject("Test");
	GameObjectTest->SetName("McLovin");
	GameObjectTest->AddComponent<SpriteRenderer>();
	//GameObjectTest->HasComponent<SpriteRenderer>();
	//GameObjectTest->RemoveComponent<SpriteRenderer>();
	//GameObjectTest->HasComponent<SpriteRenderer>();
	//GameObjectTest->AddComponent<SpriteRenderer>();
	//GameObjectTest->HasComponent<SpriteRenderer>();
	//SpriteRenderer* OwnSpriteRenderer = GameObjectTest->GetComponent<SpriteRenderer>();
	//OwnSpriteRenderer->SetEnabled(false);

	GameObjectTest->GetTransform().m_Position = Vector3f(-0.6f, 0.2f, 0.0f);
	GameObjectTest->GetTransform().m_Rotation = Vector3f(0.0f, 0.0f, 0.0f);
	GameObjectTest->GetTransform().m_Scale = Vector3f(0.5f, 0.5f, 0.5f);


	//VBO, VAO, EBO
	//const Square TestSquare;

	//VAO TestVAO;
	//TestVAO.Bind();

	//VBO TestVBO(TestSquare.m_Data, sizeof(TestSquare.m_Data));
	//EBO TestEBO(TestSquare.m_Indices, sizeof(TestSquare.m_Indices));

	//TestVBO.Bind();
	//TestEBO.Bind();

	//TestVAO.Unbind();
	//TestVBO.Unbind();
	//TestEBO.Unbind();


	while (m_Running) {
		//m_Renderer->Render();
		//const Vector3f Rotation = GameObjectTest->GetTransform().m_Rotation;
		//GameObjectTest->GetTransform().m_Rotation = Vector3f(0.0f, 0.0f, Rotation.m_Z + 0.01f);
		//ShaderProgramTest.SetUniform("uTransform", GameObjectTest->GetTransform().GetMatrix());
		//m_Renderer->TestRender(TestVAO);

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



}


void Core::RegisterExitMessage(std::string message) noexcept {
	m_ExitMessage = message;
}
void Core::PrintExitMessage() noexcept {
	printf(m_ExitMessage.data());
}