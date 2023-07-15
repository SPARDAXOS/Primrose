#pragma once
#include "Utility.hpp"

constexpr uint32 INVALID_ID = 0b00000000000000000000000000000000;
constexpr uint32 SPRITE_COMPONENT_ID = 0b00000000000000000000000000000001;
constexpr uint32 CAMERA_COMPONENT_ID = 0b00000000000000000000000000000010;

//Move somewhere else or get rid of it
constexpr uint32 MAIN_SCENE_OBJECT_ID = 0b00000000000000000000000000000000;
constexpr uint32 VIEWPORT_CAMERA_OBJECT_ID = 0b00000000000000000000000000000001;
constexpr uint32 GIZMOS_PIVOT_OBJECT_ID = 0b00000000000000000000000000000010;


class GameObject;
class ComponentBase {
public:
	ComponentBase() = delete;
	ComponentBase(GameObject& owner, uint64 ownerID) noexcept
		: m_Owner(&owner), m_OwnerID(ownerID)
	{
	}


public:
	inline void SetEnabled(bool state) noexcept { m_Enabled = state; };
	inline bool GetEnabled() const noexcept { return m_Enabled; };
	inline uint64 GetOwnerID() const noexcept { return m_OwnerID; };
	inline GameObject* GetOwner() const noexcept { return m_Owner; };


protected:
	bool m_Enabled{ true };
	uint64 m_OwnerID;
	GameObject* m_Owner;
};
