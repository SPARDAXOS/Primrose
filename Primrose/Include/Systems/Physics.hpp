#pragma once
#include "Math.hpp"


class Core;
class Camera;



struct HitResults {
	//Position of hit
	//Hit Component?
	//Hit GameObject?
	
};

class Physics final {
public:
	Physics() = delete;
	Physics(Core& core) noexcept;
	~Physics() {
		//?
	}

	Physics(const Physics&) = delete;
	Physics& operator=(const Physics&) = delete;

	Physics(Physics&&) = delete;
	Physics& operator=(Physics&&) = delete;

public:
	Vector3f ScreenToWorld(Vector2f position, float z);
	HitResults Raycast(Vector3f start, Vector3f direction, float extent);

private:
	Camera* m_ViewportCameraRef		{ nullptr };
	Core* m_CoreReference				{ nullptr };
};