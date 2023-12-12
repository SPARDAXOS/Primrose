#pragma once
#include "Utility.hpp"

constexpr uint32 INVALID_COMPONENT_ID		    = 0b00000000000000000000000000000000;
constexpr uint32 SPRITE_RENDERER_COMPONENT_ID	= 0b00000000000000000000000000000001; // 1
constexpr uint32 CAMERA_COMPONENT_ID			= 0b00000000000000000000000000000010; // 2
constexpr uint32 DIRECTIONAL_LIGHT_COMPONENT_ID = 0b00000000000000000000000000000100; // 4
constexpr uint32 POINT_LIGHT_COMPONENT_ID       = 0b00000000000000000000000000001000; // 8
constexpr uint32 SPOT_LIGHT_COMPONENT_ID		= 0b00000000000000000000000000010000; // 16
constexpr uint32 SKELETAL_MESH_COMPONENT_ID     = 0b00000000000000000000000000100000; // 32


//Move somewhere else or get rid of it
constexpr uint32 MAIN_SCENE_OBJECT_ID	   = 0b00000000000000000000000000000000;
constexpr uint32 VIEWPORT_CAMERA_OBJECT_ID = 0b00000000000000000000000000000001;
constexpr uint32 GIZMOS_PIVOT_OBJECT_ID    = 0b00000000000000000000000000000010;


class GameObject;
class ComponentBase {
public:
	ComponentBase() = delete;
	ComponentBase(GameObject& owner, uint64 ownerID) noexcept
		: m_Owner(&owner), m_OwnerID(ownerID)
	{
	}

	ComponentBase(const ComponentBase& other) {
		*this = other;
	}
	ComponentBase& operator=(const ComponentBase& other) {
		if (this == &other)
			return *this;
		else {
			//Copy All data except owner ID
			this->m_Enabled = other.m_Enabled;
			return *this;
		}
	}

	ComponentBase(ComponentBase&& other) {
		*this = std::move(other);
	}
	ComponentBase& operator=(ComponentBase&& other) {
		if (this == &other)
			return *this;
		else {
			//Move All data including owner id and invalidate the previous? Delete it instead?
			this->m_Enabled = other.m_Enabled;
			this->m_OwnerID = other.m_OwnerID;
			this->m_Owner   = other.m_Owner;

			other.m_Enabled = true;
			other.m_OwnerID = 0; //INVALID_OBJECT_ID
			other.m_Owner   = nullptr;

			//Im worried about the fact that all pointers are invalidated now everytime the list grows!

			return *this;
		}
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
