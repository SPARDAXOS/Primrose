#pragma once
#include "Utility.hpp"
#include "Component.hpp"

class EntityComponentSystem;

class GameObject {
public:

	GameObject() = delete;
	GameObject(const EntityComponentSystem& ecs, uint32 id)
		:	m_ECS(&ecs), m_ObjectID(id)
	{
	}


public:
	//TODO: Add check that T inherits from ComponentBase
	template<typename T>
	T AddComponent() {
		static_assert(std::is_base_of_v<ComponentBase, T>);
		//Check if has comp already
	}

	template<typename T>
	void RemoveComponent() {
		static_assert(std::is_base_of_v<ComponentBase, T>);
		//Check if has comp already
	}

	template<typename T>
	T GetComponent() {
		static_assert(std::is_base_of_v<ComponentBase, T>);
		//Check if has comp already
	}

	template<typename T>
	bool HasComponent() {

		static_assert(std::is_base_of_v<ComponentBase, T>);
		uint32 ComponentID = ComponentBase::GetComponentID<T>();
		if (ComponentID == INVALID_ID)
			return false;

		if ((m_ComponentFlags &= ComponentID) == 1)
			return true;
		else
			return false;
	}

public:

	inline uint64 GetObjectID() const noexcept { return m_ObjectID; };
	inline std::string GetObjectName() const noexcept { return m_ObjectName; };

	inline bool IsEnabled() const noexcept { return m_Enabled; };
	inline GameObject* GetParent() const noexcept { return m_Parent; };


	void SetObjectName(std::string name) noexcept;
	void SetEnabled(bool state) noexcept;
	void SetParent(GameObject* parent) noexcept;

private:




private:
	bool m_Enabled{ true };
	bool m_Static{ false };

private:
	const EntityComponentSystem* m_ECS;
	GameObject* m_Parent{ nullptr };
	uint32 m_ComponentFlags{ 0 };
	uint64 m_ObjectID{ 0 };
	std::string m_ObjectName{ "NewGameObject" };
};