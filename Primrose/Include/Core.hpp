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


*/


class Core final {
public:
	explicit Core();

private:
	void SetupCore();


public:
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<Window> m_Window;

};