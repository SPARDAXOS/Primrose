#include "Sorting.h"
#include <chrono>
#include <functional>
#include <iostream>

#ifndef PROFILER
#define PROFILER

namespace {

	using Timepoint = std::chrono::high_resolution_clock::time_point;
	using Duration = std::chrono::high_resolution_clock::duration;
	using Microseconds = std::chrono::microseconds;
	using Milliseconds = std::chrono::milliseconds;
	using Seconds = std::chrono::seconds;
	using Minutes = std::chrono::minutes;
	using Hours = std::chrono::hours;
}

class Profile final {
public:
	explicit Profile() noexcept = default;
	Profile(std::string_view id, Timepoint start)
		: m_ID(id), m_StartingTimepoint(start)
	{
	}
	Profile(std::string_view id, Timepoint start, Timepoint end, Duration duration)
		: m_ID(id), m_StartingTimepoint(start), m_EndTimepoint(end), m_Duration(duration)
	{
	}

	Profile(const Profile& other) = default;
	Profile& operator=(const Profile& other) = default;

	Profile(Profile&& other) noexcept = default;
	Profile& operator=(Profile&& other) noexcept = default;

	bool operator<(const Profile& other) const noexcept {
		return this->m_Duration < other.m_Duration;
	}
	bool operator>(const Profile& other) const noexcept {
		return this->m_Duration > other.m_Duration;
	}

public:
	inline Microseconds DurationAsMicroseconds() const noexcept { return std::chrono::duration_cast<Microseconds>(m_Duration); }
	inline Milliseconds DurationAsMilliseconds() const noexcept { return std::chrono::duration_cast<Milliseconds>(m_Duration); }
	inline Seconds DurationAsSeconds() const noexcept { return std::chrono::duration_cast<Seconds>(m_Duration); }

public:
	std::string_view m_ID;
	Timepoint m_StartingTimepoint;
	Timepoint m_EndTimepoint;
	Duration m_Duration{ 0 };
};
class IterativeProfile final {
public:
	IterativeProfile() noexcept = default;
	IterativeProfile(Timepoint start)
		: m_StartingTimepoint(start)
	{
	}

	IterativeProfile(const IterativeProfile& other) = default;
	IterativeProfile& operator=(const IterativeProfile& other) = default;

	IterativeProfile(IterativeProfile&& other) noexcept = default;
	IterativeProfile& operator=(IterativeProfile&& other) noexcept = default;

	void operator+(const IterativeProfile& other) noexcept {
		this->m_StartingTimepoint += other.m_StartingTimepoint.time_since_epoch();
		this->m_EndTimepoint += other.m_EndTimepoint.time_since_epoch();
		this->m_Duration += other.m_Duration;
		this->m_Average += other.m_Average;
		this->m_Median += other.m_Median;
		this->m_Max += other.m_Max;
		this->m_Min += other.m_Min;
		this->m_Count += other.m_Count;

		for (auto& element : other.m_Profiles)
			this->m_Profiles.emplace_back(element);
	}
	void operator+=(const IterativeProfile& other) noexcept {
		*this + other;
	}

public:
	inline Microseconds DurationAsMicroseconds() const noexcept { return std::chrono::duration_cast<Microseconds>(m_Duration); }
	inline Microseconds AverageAsMicroseconds()  const noexcept { return std::chrono::duration_cast<Microseconds>(m_Average); }
	inline Microseconds MedianAsMicroseconds()	 const noexcept { return std::chrono::duration_cast<Microseconds>(m_Median); }
	inline Microseconds MaxAsMicroseconds()		 const noexcept { return std::chrono::duration_cast<Microseconds>(m_Max); }
	inline Microseconds MinAsMicroseconds()		 const noexcept { return std::chrono::duration_cast<Microseconds>(m_Min); }

	inline Milliseconds DurationAsMilliseconds() const noexcept { return std::chrono::duration_cast<Milliseconds>(m_Duration); }
	inline Milliseconds AverageAsMilliseconds()  const noexcept { return std::chrono::duration_cast<Milliseconds>(m_Average); }
	inline Milliseconds MedianAsMilliseconds()   const noexcept { return std::chrono::duration_cast<Milliseconds>(m_Median); }
	inline Milliseconds MaxAsMilliseconds()		 const noexcept { return std::chrono::duration_cast<Milliseconds>(m_Max); }
	inline Milliseconds MinAsMilliseconds()      const noexcept { return std::chrono::duration_cast<Milliseconds>(m_Min); }

	inline Seconds DurationAsSeconds()			 const noexcept { return std::chrono::duration_cast<Seconds>(m_Duration); }
	inline Seconds AverageAsSeconds()			 const noexcept { return std::chrono::duration_cast<Seconds>(m_Average); }
	inline Seconds MedianAsSeconds()			 const noexcept { return std::chrono::duration_cast<Seconds>(m_Median); }
	inline Seconds MaxAsSeconds()				 const noexcept { return std::chrono::duration_cast<Seconds>(m_Max); }
	inline Seconds MinAsSeconds()				 const noexcept { return std::chrono::duration_cast<Seconds>(m_Min); }

public:
	Timepoint m_StartingTimepoint;
	Timepoint m_EndTimepoint;
	Duration m_Duration{ 0 };
	Duration m_Average{ 0 };
	Duration m_Median{ 0 };
	Duration m_Max{ 0 };
	Duration m_Min{ 0 };
	uint32 m_Count{ 0 };
	std::vector<Profile> m_Profiles;
};

class Profiler final {
public:
	using Predicate = std::function<void(void)>;

public:
	explicit Profiler() noexcept = default;
	~Profiler() noexcept = default;

	Profiler(const Profiler& other) = default;
	Profiler& operator=(const Profiler& other) = default;

	Profiler(Profiler&& other) noexcept = default;
	Profiler& operator=(Profiler&& other) noexcept = default;

public:
	inline bool StartProfile(std::string_view id) {
		if (FindUnit(std::string(id)).has_value()) {
			std::cout << "Profile unit with id " << id << " already exists!" << std::endl;
			return false;
		}

		m_ProfillingUnits.emplace_back(id, m_Clock.now());
		m_RunningProfiles++;
		return true;
	}
	[[nodiscard]] inline std::optional<Profile> StopProfile(std::string_view id) noexcept {
		auto StoppingTime = m_Clock.now();
		auto TargetUnit = FindUnit(std::string(id));
		if (!TargetUnit.has_value()) {
			std::cout << "Failed to find and end profiling unit designated " << id << std::endl;
			return std::optional<Profile>(std::nullopt);
		}

		TargetUnit->first.m_EndTimepoint = StoppingTime;
		TargetUnit->first.m_Duration = TargetUnit->first.m_EndTimepoint - TargetUnit->first.m_StartingTimepoint;

		m_ProfillingUnits.erase(m_ProfillingUnits.begin() + TargetUnit->second);
		m_RunningProfiles--;
		return TargetUnit->first;
	}
	[[nodiscard]] inline IterativeProfile RunIterativeProfile(Predicate block, uint32 iterations = 1) {
		if (iterations == 0)
			return {};

		IterativeProfile ResultsProfile;
		ResultsProfile.m_Count = iterations;
		ResultsProfile.m_Profiles.reserve(iterations);
		ResultsProfile.m_StartingTimepoint = m_Clock.now();
		
		for (uint32 i = 0; i < iterations; i++) {
			Profile NewProfile;
			NewProfile.m_StartingTimepoint = m_Clock.now();
			block();
			NewProfile.m_EndTimepoint = m_Clock.now();
			NewProfile.m_Duration = NewProfile.m_EndTimepoint - NewProfile.m_StartingTimepoint;
			ResultsProfile.m_Duration += NewProfile.m_Duration;
			ResultsProfile.m_Profiles.emplace_back(NewProfile);

			if (i == 0) {
				ResultsProfile.m_Max = NewProfile.m_Duration;
				ResultsProfile.m_Min = NewProfile.m_Duration;
				continue;
			}

			if (ResultsProfile.m_Min > NewProfile.m_Duration)
				ResultsProfile.m_Min = NewProfile.m_Duration;
			if (ResultsProfile.m_Max < NewProfile.m_Duration)
				ResultsProfile.m_Max = NewProfile.m_Duration;
		}

		ResultsProfile.m_EndTimepoint = m_Clock.now();

		CalculateAverage(ResultsProfile);
		CalculateMedian(ResultsProfile);

		return ResultsProfile;
	}

public:
	constexpr inline void CalculateAverage(IterativeProfile& profile) const noexcept {
		profile.m_Average = profile.m_Duration / profile.m_Profiles.size();
	}
	constexpr inline void CalculateMedian(IterativeProfile& profile) const noexcept {

		if (profile.m_Profiles.size() == 1)
			profile.m_Median = profile.m_Profiles[0].m_Duration;
		else if (profile.m_Profiles.size() == 2) {
			auto Element1 = profile.m_Profiles[0].m_Duration;
			auto Element2 = profile.m_Profiles[1].m_Duration;
			profile.m_Median = (Element1 + Element2) / 2;
		}
		else {
			Sorting::QuickSort(profile.m_Profiles.begin()._Ptr, profile.m_Profiles.end()._Ptr);
			if (profile.m_Profiles.size() % 2 == 1)
				profile.m_Median = profile.m_Profiles[(profile.m_Profiles.size() + 1) / 2].m_Duration;
			else if (profile.m_Profiles.size() % 2 == 0) {
				auto Index1 = profile.m_Profiles.size() / 2 - 1;
				auto Index2 = (profile.m_Profiles.size() / 2 - 1) + 1;

				auto Duration1 = profile.m_Profiles[Index1].m_Duration;
				auto Duration2 = profile.m_Profiles[Index2].m_Duration;
				profile.m_Median = (Duration1 + Duration2) / 2;
			}
		}
	}

private:
	constexpr inline std::optional<std::pair<Profile, uint32>> FindUnit(std::string id) const noexcept {
		for (uint32 i = 0; i < m_ProfillingUnits.size(); i++) {
			if (m_ProfillingUnits[i].m_ID == id)
				return std::optional<std::pair<Profile, uint32>>({ m_ProfillingUnits[i], i });
		}

		return std::optional<std::pair<Profile, uint32>>(std::nullopt);
	}
	
private:
	std::chrono::high_resolution_clock m_Clock;
	std::vector<Profile> m_ProfillingUnits;
	uint32 m_RunningProfiles = 0;
};
#endif // !PROFILER
