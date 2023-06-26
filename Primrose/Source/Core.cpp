#include <Core.hpp>


Core::Core() {

	//TODO: is error checking needed here? probably not since they throw and i should just let it bubble up.

	m_Window = std::make_unique<Window>();
	m_Renderer = std::make_unique<Renderer>(*m_Window.get());
	m_TextureStorage = std::make_unique<TextureStorage>();
}


void Core::SetupCore() { // Sounds like 2 step initialization

}


void Core::Run() {
	//Run all operations before simply updating the systems in a loop
	m_Running = true;


	//Shaders
	Shader VertexShader(GL_VERTEX_SHADER, "Resources/Shaders/Vertex.glsl");
	Shader FragmentShader(GL_FRAGMENT_SHADER, "Resources/Shaders/Fragment.glsl");

	ShaderProgram ShaderProgramTest;
	ShaderProgramTest.AttachShader(VertexShader);
	ShaderProgramTest.AttachShader(FragmentShader);
	if (!ShaderProgramTest.LinkShaderProgram())
		PrintMessage("LinkShaderProgram Failed");
	ShaderProgramTest.Bind();

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

	ShaderProgramTest.SetUniform("uDiffuse", TextureUnit::DIFFUSE);


	//Creat matrix out of the Translation, Rotation and Scale vectors using this 

	//Transformations
	Transform TransformTest;
	TransformTest.m_Position = Vector3f(-0.6f, 0.2f, 0.0f);
	TransformTest.m_Rotation = Vector3f(0.0f, 0.0f, 90.0f);
	TransformTest.m_Scale    = Vector3f(0.5f, 0.5f, 0.5f);
	TransformTest.UpdateMatrix();


	//VBO, VAO, EBO
	const Square TestSquare;

	VAO TestVAO;
	TestVAO.Bind();

	VBO TestVBO(TestSquare.m_Data, sizeof(TestSquare.m_Data));
	EBO TestEBO(TestSquare.m_Indices, sizeof(TestSquare.m_Indices));

	TestVBO.Bind();
	TestEBO.Bind();

	TestVAO.Unbind();
	TestVBO.Unbind();
	TestEBO.Unbind();


	while (m_Running) {
		m_Renderer->Clear();
		//m_Renderer->Render();


		ShaderProgramTest.SetUniform("uTransform", TransformTest.GetMatrix());
		m_Renderer->TestRender(TestVAO);


		m_Renderer->SwapBuffers(); //Questionable since it calls a func at the window so why not just call the window one

		UpdateSystems();
	}

	Exit();
	//End all operations then clean up
}
void Core::Exit() {

	PrintExitMessage();
}
void Core::UpdateSystems() {
	
	if (!m_Window->UpdateWindow()) {
		RegisterExitMessage("Engine closed down.\nReason: " + m_Window->GetLastExitMessage());
		m_Running = false; //TODO: make cleaner exit method + some messages for error handling
	}




}


void Core::RegisterExitMessage(std::string message) noexcept {
	m_ExitMessage = message;
}
void Core::PrintExitMessage() noexcept {
	printf(m_ExitMessage.data());
}