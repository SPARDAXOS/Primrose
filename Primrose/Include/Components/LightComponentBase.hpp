#pragma once
#include "Component.hpp"
#include "Math.hpp"
#include "Transform.hpp"

class GameObject;



class LightComponentBase : public ComponentBase {
public:

	LightComponentBase() = delete;
	LightComponentBase(GameObject& owner, uint64 ownerID);
	~LightComponentBase() = default;

	//For now it is not possbile to move or copy components - Copy only it should be? idk think about it
	LightComponentBase(const LightComponentBase& other) = delete;
	LightComponentBase& operator=(const LightComponentBase& other) = delete;

	LightComponentBase(LightComponentBase&& other) = delete;
	LightComponentBase& operator=(LightComponentBase&& other) = delete;

public:
	Color m_Tint = Colors::White;

};