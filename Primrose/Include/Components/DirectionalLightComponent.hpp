#pragma once
#include "LightComponentBase.hpp"

class DirectionalLight final : public LightComponentBase {
public:
	DirectionalLight() = delete;
	DirectionalLight(GameObject& owner, uint64 ownerID);
	~DirectionalLight() = default;

	//For now it is not possbile to move or copy components - Copy only it should be? idk think about it
	DirectionalLight(const DirectionalLight& other) = delete;
	DirectionalLight& operator=(const DirectionalLight& other) = delete;

	DirectionalLight(DirectionalLight&& other) = delete;
	DirectionalLight& operator=(DirectionalLight&& other) = delete;

public:
	Vector4f GetDirection() const noexcept;


};