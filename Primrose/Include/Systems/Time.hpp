#pragma once

#include "GLFW/glfw3.h"
#include <chrono>
#include <iostream>

class Time final {
public:
	void Update() noexcept {
		const double CurrentFrameTime = glfwGetTime();
		m_DeltaTime = CurrentFrameTime - m_LastFrameTime;

		//Looks weird - Maybe ditch glfw and use chrono?
		m_LastFrameTime = CurrentFrameTime;
		m_RunningTime = static_cast<uint64>(CurrentFrameTime);
		m_Buffer[m_BufferIndex] = m_DeltaTime;
		m_BufferIndex = (m_BufferIndex + 1) % 50;

		m_Tick++;
	}

public:
	inline double GetDeltaTime() const noexcept {
		double Sum = 0;
		for (uint32 index = 0; index < 50; index++)
			Sum += m_Buffer[index];
		
		Sum /= 50;
		return Sum;
	}
	inline uint32 GetFPS() const noexcept {
		const double DeltaTime = GetDeltaTime();
		return static_cast<unsigned int>(1 / DeltaTime);
	}
	inline uint64 GetRunningTime() const noexcept { return m_RunningTime; }
	std::string GetRunningTimeFormatted() const {

		const std::chrono::hh_mm_ss TimeFormatted{std::chrono::seconds(m_RunningTime)};
		std::string Results;

		Results.append(std::to_string(TimeFormatted.hours().count()));
		Results.append(":");
		Results.append(std::to_string(TimeFormatted.minutes().count()));
		Results.append(":");
		Results.append(std::to_string(TimeFormatted.seconds().count()));
		
		return Results;
	}
	std::string FormatTime(uint64 seconds) {

		const std::chrono::hh_mm_ss TimeFormatted{std::chrono::seconds(seconds)};
		std::string Results;

		Results.append(std::to_string(TimeFormatted.hours().count()));
		Results.append(":");
		Results.append(std::to_string(TimeFormatted.minutes().count()));
		Results.append(":");
		Results.append(std::to_string(TimeFormatted.seconds().count()));

		return Results;
	}

	inline uint64 GetTick() const noexcept { return m_Tick; }

private:
	double m_Buffer[50];
	uint32 m_BufferIndex = 0;
	double m_LastFrameTime;

private:
	double m_DeltaTime = 0.0;
	uint64 m_RunningTime = 0;
	uint64 m_Tick = 0;
};