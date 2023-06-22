#include <Core.hpp>


Core::Core() {

	//TODO: is error checking needed here? probably not since they throw and i should just let it bubble up.

	m_Window = std::make_unique<Window>();
	m_Renderer = std::make_unique<Renderer>(*m_Window.get());
	m_FileManager = std::make_unique<FileManager>();
	m_ShaderCompiler = std::make_unique<ShaderCompiler>(*m_FileManager.get());

}


void Core::SetupCore() { // Sounds like 2 step initialization

}


void Core::Run() {
	//Run all operations before simply updating the systems in a loop
	m_Running = true;

	std::string VertexShaderSource;
	std::string FragmentShaderSource;
	GLuint VertexShaderID = 0;
	GLuint FragmentShaderID = 0;
	GLuint ShaderProgramID = 0;

	
	//Shaders
	if (!m_ShaderCompiler->LoadShader("Resources/Shaders/Vertex.txt", VertexShaderSource))
		PrintMessage("LoadShader Failed at VertexShader");
	if (!m_ShaderCompiler->LoadShader("Resources/Shaders/Fragment.txt", FragmentShaderSource))
		PrintMessage("LoadShader Failed at FragmentShader");

	if (!m_ShaderCompiler->CompileShader(VertexShaderID, GL_VERTEX_SHADER, VertexShaderSource))
		PrintMessage("CompileShader Failed at VertexShader");
	if (!m_ShaderCompiler->CompileShader(FragmentShaderID, GL_FRAGMENT_SHADER, FragmentShaderSource))
		PrintMessage("CompileShader Failed at FragmentShader");

	if (!m_ShaderCompiler->CreateShaderProgram(ShaderProgramID))
		PrintMessage("CreateShaderProgram Failed");

	m_ShaderCompiler->AttachShader(VertexShaderID, ShaderProgramID);
	m_ShaderCompiler->AttachShader(FragmentShaderID, ShaderProgramID);

	if (!m_ShaderCompiler->LinkShaderProgram(ShaderProgramID))
		PrintMessage("LinkShaderProgram Failed");

	GLCall(glUseProgram(ShaderProgramID));


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