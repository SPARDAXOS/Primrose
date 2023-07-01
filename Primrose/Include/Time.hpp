#pragma once
#include "GLFW/glfw3.h"

class Time final {
public:
	void Update() noexcept {
		const double CurrentFrameTime = glfwGetTime();
		m_DeltaTime = CurrentFrameTime - m_LastFrameTime;

		//Looks weird - Maybe ditch glfw and use chrono?
		m_LastFrameTime = CurrentFrameTime;
		m_RunningTime = CurrentFrameTime;
		m_Buffer[m_BufferIndex] = m_DeltaTime;
		m_BufferIndex = (m_BufferIndex + 1) % 50;
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
	inline double GetRunningTime() const noexcept { return m_RunningTime; }

private:
	double m_Buffer[50];
	uint32 m_BufferIndex = 0;
	double m_LastFrameTime;

private:
	double m_DeltaTime;
	double m_RunningTime;
};