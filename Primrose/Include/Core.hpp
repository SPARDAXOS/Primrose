#pragma once
#include <stdexcept>
#include <memory>

#include "Renderer.hpp"
#include "Window.hpp"
#include "Utility.hpp"


/*
TODO: Implement RendererBackend
TODO: Implement Renderer
TODO: Implement TextureStorage
TODO: Implement ECS

//Test Comment
*/


class Core final {
public:
	explicit Core();

	void Run();

private:
	void SetupCore();


	//TODO: Rule of 6 - define special member functions 

	void UpdateSystems();

private:
	bool m_Running = false;

public:
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<Window> m_Window;


};