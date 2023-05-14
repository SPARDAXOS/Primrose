#include <Core.hpp>


Core::Core() {
	try {

		m_Renderer = std::make_unique<Renderer>();
		m_Window = std::make_unique<Window>();


	}
	catch (const std::exception& exception) {
		PrintMessage(exception.what());
	}
}


void Core::SetupCore() {
}