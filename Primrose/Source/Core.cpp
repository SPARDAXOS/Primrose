#include <Core.hpp>


Core::Core() {
	
	m_Renderer = std::make_unique<Renderer>();
	m_Window = std::make_unique<Window>();

}


void Core::SetupCore() {

}


void Core::Run() {
	//Run all operations before simply updating the systems in a loop
	m_Running = true;

	while (m_Running) {

		UpdateSystems();
	}

	//End all operations then clean up
}
void Core::UpdateSystems() {
	
	if (!m_Window->UpdateWindow()) {
		m_Running = false; //TODO: make cleaner exit method + some messages for error handling
	}
}