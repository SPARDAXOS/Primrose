#pragma once
#include "Utility.hpp"

//TODO: move this somewhere else. Its own header probably
class ComponentBase {

};



class GameObject {
public:

	//TODO: Add check that T inherits from ComponentBase
	template<typename T>
	void AddComponent() {
		static_assert(std::is_base_of_v<ComponentBase, T>);

	}

	template<typename T>
	void RemoveComponent() {
		static_assert(std::is_base_of_v<ComponentBase, T>);

	}

	template<typename T>
	T GetComponent() {
		static_assert(std::is_base_of_v<ComponentBase, T>);

	}

	template<typename T>
	bool HasComponent() {
		static_assert(std::is_base_of_v<ComponentBase, T>);

	}


public:
	bool m_Enabled{ true };


private:
	bool m_Static{ false };

private:
	uint32 m_ComponentFlags{ 0 };
	uint64 m_ObjectID{ 0 };
	//Ref to ECS? Should not be accessable through this class at all.
};