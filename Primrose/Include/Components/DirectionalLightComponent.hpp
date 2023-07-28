#pragma once
#include "LightComponentBase.hpp"

class DirectionalLightComponent final : public LightComponentBase {
public:

	DirectionalLightComponent() = delete;
	DirectionalLightComponent(GameObject& owner, uint64 ownerID);
	~DirectionalLightComponent() = default;

	//For now it is not possbile to move or copy components - Copy only it should be? idk think about it
	DirectionalLightComponent(const DirectionalLightComponent& other) = delete;
	DirectionalLightComponent& operator=(const DirectionalLightComponent& other) = delete;

	DirectionalLightComponent(DirectionalLightComponent&& other) = delete;
	DirectionalLightComponent& operator=(DirectionalLightComponent&& other) = delete;



};