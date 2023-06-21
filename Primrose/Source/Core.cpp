#include <Core.hpp>


Core::Core() {
	
	//TODO: is error checking needed here? probably not since they throw and i should just let it bubble up.

	m_Window = std::make_unique<Window>();
	m_Renderer = std::make_unique<Renderer>(*m_Window.get());
	m_ShaderCompiler = std::make_unique<ShaderCompiler>();
	m_FileManager = std::make_unique<FileManager>();

}


void Core::SetupCore() { // Sounds like 2 step initialization

}


void Core::Run() {
	//Run all operations before simply updating the systems in a loop
	m_Running = true;

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