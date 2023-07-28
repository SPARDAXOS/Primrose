#include "Tools/Profiler.hpp"
#include <iostream>

bool Profiler::StartProfile(const char* id) {
	//TODO: add error messages

	//if (!AnyProfilesRunning()) {
	//	return false;
	//}
	if (FindProfile(id) != -1) { //Dup
		return false;
	}

	m_StartingTimes.push_back(ProfileTimePoint(std::chrono::high_resolution_clock::now()));
	m_IDs.push_back(id);

	return true;
}
void Profiler::EndProfile(const char* id) {
	//TODO: add error messages

	if (!AnyProfilesRunning()) {
		return;
	}

	const int TargetProfileIndex = FindProfile(id); //TODO: change approach to not rely on int but unsigned int ref as argument
	if (TargetProfileIndex == -1) { //Not found
		return;
	}

	const auto Results = std::chrono::high_resolution_clock::now() - m_StartingTimes[TargetProfileIndex];
	std::cout << Results.count() << "ns -Results of the profile (" << id << ")" << "\n";

	//TODO: delete profile from times and ids vectors
}

bool Profiler::AnyProfilesRunning() const noexcept {
	return m_StartingTimes.size() > 0;
}
int Profiler::FindProfile(const char* id) const {
	if (!AnyProfilesRunning()) {
		return -1;
	}

	for (unsigned int index = 0; index < m_IDs.size(); index++) {
		if (m_IDs[index] == id) {
			return index;
		}
	}

	return -1;
}
void Profiler::RemoveProfile(unsigned int index) {

}