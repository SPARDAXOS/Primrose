#pragma once
#include <vector>
#include "Utility.hpp"



//Break into heirarchy then clean coupled code from editor

enum class MessageType {
	DEBUG,
	WARNING,
	ERROR,
	SYSTEM
};

class Message {
public:
	Message() = delete;
	~Message() = default;
	Message(MessageType type, uint64 tick, uint64 timestamp, std::string data) noexcept
		:	m_Type(type), m_Tick(tick), m_Timestamp(timestamp), m_Data(data)
	{
	}

	Message(const Message& other) noexcept {
		*this = other;
	}
	Message& operator=(const Message& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_Type = other.m_Type;
			this->m_Tick = other.m_Tick;
			this->m_Timestamp = other.m_Timestamp;
			this->m_Data = other.m_Data;
			return *this;
		}
	}

	Message(Message&& other) noexcept {
		*this = std::move(other);
	}
	Message& operator=(Message&& other) noexcept {
		if (*this == other) {
			return *this;
		}
		else {
			this->m_Type = std::move(other.m_Type);
			this->m_Tick = std::move(other.m_Tick);
			this->m_Timestamp = std::move(other.m_Timestamp);
			this->m_Data = std::move(other.m_Data);
			return *this;
		}
	}

	bool operator==(const Message& other) const noexcept {

		if (m_Type != other.m_Type)
			return false;
		else if (m_Tick != other.m_Tick)
			return false;
		else if (m_Timestamp != other.m_Timestamp)
			return false;
		else if (m_Data != other.m_Data)
				return false;
		else
			return true;
	}
	bool operator!=(const Message& other) const noexcept {
		if (!(*this == other))
			return true;
		else
			return false;
	}

public:
	MessageType GetType() const noexcept { return m_Type; }
	uint64      GetTick() const noexcept { return m_Tick; }
	uint64		GetTimestamp() const noexcept{ return m_Timestamp; }
	std::string GetData() const noexcept { return m_Data; }

private:
	MessageType m_Type = MessageType::DEBUG;
	uint64 m_Tick = 0;
	uint64 m_Timestamp = 0;
	std::string m_Data;
};



class Time;

class Logger final {
public:
	Logger() = delete;
	Logger(Time& time) noexcept;
	~Logger() = default;

	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

	Logger(Logger&&) = delete;
	Logger& operator=(Logger&&) = delete;

public:
	void LogDebug(std::string message) noexcept;
	void LogWarning(std::string message) noexcept;
	void LogError(std::string message) noexcept;
	void LogSystem(std::string message) noexcept;

public:
	inline Message* GetNextLoggedMessage() noexcept { 

		if (m_DebugLog.size() == 0 || m_IsLoggedBufferLooped)
			return nullptr;

		Message* message = &m_DebugLog.at(m_LoggedMessageCursor);
		m_LoggedMessageCursor++;
		if (m_LoggedMessageCursor == m_DebugLog.size()) {
			m_LoggedMessageCursor = 0;
			m_IsLoggedBufferLooped = true;
		}
		return message;
	}
	inline Message* GetNextSystemMessage() noexcept {

		if (m_SystemLog.size() == 0 || m_IsSystemBufferLooped)
			return nullptr;

		Message* message = &m_SystemLog.at(m_SystemMessageCursor);
		m_SystemMessageCursor++;
		if (m_SystemMessageCursor == m_SystemLog.size()) {
			m_SystemMessageCursor = 0;
			m_IsSystemBufferLooped = true;
		}
		return message;
	}

	inline void NewFrame() noexcept { 

		m_IsLoggedBufferLooped = false; 
		m_IsSystemBufferLooped = false; 
		//In case of API calls misuse
		m_LoggedMessageCursor = 0;
		m_SystemMessageCursor = 0;
	}
	inline void ClearMessageLog() noexcept {

		m_DebugCounter = 0;
		m_WarningCounter = 0;
		m_ErrorCounter = 0;
		m_DebugLog.clear();
	}
	inline void ClearSystemLog() noexcept {

		m_SystemCounter = 0;
		m_SystemLog.clear();
	}
	inline bool IsLoggedBufferLooped() noexcept { return m_IsLoggedBufferLooped; }
	inline bool IsSystemBufferLooped() noexcept { return m_IsSystemBufferLooped; }

public:
	inline uint32 GetLoggedMessagesCount() const noexcept { return m_DebugCounter + m_WarningCounter + m_ErrorCounter; }
	inline uint32 GetSystemMessagesCount() const noexcept { return m_SystemCounter; } //In case i add more system message types later on

	inline uint32 GetDebugMessagesCount() const noexcept { return m_DebugCounter; }
	inline uint32 GetWarningMessagesCount() const noexcept { return m_WarningCounter; }
	inline uint32 GetErrorMessagesCount() const noexcept { return m_ErrorCounter; }
	//I can divide the system messages into types once i have more than 1 type

public:
	inline bool GetShowDebugMessages() const noexcept { return m_ShowDebugMessages; }
	inline bool GetShowWarningMessages() const noexcept { return m_ShowWarningMessages; }
	inline bool GetShowErrorMessages() const noexcept { return m_ShowErrorMessages; }
	inline bool GetShowSystemMessages() const noexcept { return m_ShowErrorMessages; }

	inline bool GetAutoScrollDebugLog() const noexcept { return m_AutoScrollDebugLog; }
	inline bool GetAutoScrollSystemLog() const noexcept { return m_AutoScrollSystemLog; }

	inline void ToggleShowDebugMessages() noexcept { m_ShowDebugMessages ^= 1; }
	inline void ToggleShowWarningMessages() noexcept { m_ShowWarningMessages ^= 1; }
	inline void ToggleShowErrorMessages() noexcept { m_ShowErrorMessages ^= 1; }
	inline void ToggleShowSystemMessages() noexcept { m_ShowErrorMessages ^= 1; }

	inline void ToggleAutoScrollDebugLog() noexcept { m_AutoScrollDebugLog ^= 1; }
	inline void ToggleAutoScrollSystemLog() noexcept { m_AutoScrollSystemLog ^= 1; }

	inline void SetTimeReference(Time& time) noexcept { m_TimeReference = &time; }

private:
	bool m_ShowDebugMessages   = true;
	bool m_ShowWarningMessages = true;
	bool m_ShowErrorMessages   = true;

	bool m_AutoScrollDebugLog = true;
	bool m_AutoScrollSystemLog = true;

	bool m_IsLoggedBufferLooped = false;
	bool m_IsSystemBufferLooped = false;

private:
	uint32 m_DebugLimit     = 999;
	uint32 m_WarningLimit   = 100;
	uint32 m_ErrorLimit     = 100;
	uint32 m_SystemLimit    = 200; //Should probably not have a cap even

	uint32 m_DebugCounter   = 0;
	uint32 m_WarningCounter = 0;
	uint32 m_ErrorCounter   = 0;
	uint32 m_SystemCounter  = 0;

	uint32 m_LoggedMessageCursor = 0;
	uint32 m_SystemMessageCursor = 0;

private:
	Time* m_TimeReference = nullptr;
	//I dont like the differnciation between debug and system especially since they both are the same struct and the enum encompasses both
	//NOTE: Maybe seperate them into 2 message types that each inherit from a base message and has only different types as unique member variable
	std::vector<Message> m_DebugLog;
	std::vector<Message> m_SystemLog;
};