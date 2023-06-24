#include <Core.hpp>


Core::Core() {

	//TODO: is error checking needed here? probably not since they throw and i should just let it bubble up.

	m_Window = std::make_unique<Window>();
	m_Renderer = std::make_unique<Renderer>(*m_Window.get());

}


void Core::SetupCore() { // Sounds like 2 step initialization

}


void Core::Run() {
	//Run all operations before simply updating the systems in a loop
	m_Running = true;


	//Shaders
	Shader VertexShader(GL_VERTEX_SHADER, "Resources/Shaders/Vertex.txt");
	Shader FragmentShader(GL_FRAGMENT_SHADER, "Resources/Shaders/Fragment.txt");

	ShaderProgram ShaderProgramTest;
	ShaderProgramTest.AttachShader(VertexShader);
	ShaderProgramTest.AttachShader(FragmentShader);
	if (!ShaderProgramTest.LinkShaderProgram())
		PrintMessage("LinkShaderProgram Failed");
	ShaderProgramTest.Bind();

	//Textures
	
	//TODO: Implement own image loader!


	std::string testImage;
	//if (!FileManagement::CRead("Resources/Textures/Crate.jpg", testImage))
	//	PrintMessage("It failed to load the image");
	Texture test;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load("Resources/Textures/Crate.jpg", &test.m_Width, &test.m_Height, &test.m_ColorChannelsCount, 0);

	//ImageLoader::ParseJPG("Resources/Textures/Crate.jpg", testImage);



	GLuint TextureTest;
	glGenTextures(1, &TextureTest);
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, TextureTest));

	//For Both axis
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));

	//For magnifying and minifying textures - When texture is smaller than object or bigger than object!
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)); // For mipmaps. Cuase gets smaller while the magnifying doesnt use mipmaps so dont use it!
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, test.m_Width, test.m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
	//GLCall(glGenerateMipmap(TextureTest));
	glGenerateMipmap(GL_TEXTURE_2D);

	GLCall(glUniform1i(glGetUniformLocation(ShaderProgramTest.GetID(), "uTexture"), 0));

	stbi_image_free(data);

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



	//Quick Animation
	float c = 0.0f;
	bool up = true;
	Color TestColor{ 0.0f, 0.0f, 0.0f, 1.0f };


	while (m_Running) {


		//ShaderProgramTest.SetUniform("u_Color", TestColor);

		if (up) {
			c += 0.01f;
			TestColor.R = c;
			if (c > 1.0f) {
				c = 1.0f;
				TestColor.R = 0.0f;
				up = false;
			}
		}
		else {
			c -= 0.005f;
			TestColor.B = c;
			if (c < 0.0f) {
				c = 0.0f;
				TestColor.B = 0.0f;
				up = true;
			}
		}

		m_Renderer->Render();
		m_Renderer->TestRender(TestVAO);
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