#pragma once
#include <chrono>


//TODO: Implement Profile ID clamping to avoid exploit with long IDs.


class Profiler final {
	using ProfileTimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

public:
	Profiler() noexcept {

	}

public:
	bool StartProfile(const char* id);
	void EndProfile(const char* id);

private:
	bool AnyProfilesRunning() const noexcept;
	int FindProfile(const char* id) const;
	void RemoveProfile(unsigned int index);

private:
	std::vector<ProfileTimePoint> m_StartingTimes;
	std::vector<const char*> m_IDs;
};