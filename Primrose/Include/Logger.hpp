#pragma once
#include <vector>
#include "Utility.hpp"


enum class MessageType {
	DEBUG,
	WARNING,
	ERROR
};

class Message {
public:
	Message() = delete;
	~Message() = default;
	Message(MessageType type, uint64 tick, const char* data) noexcept
		:	m_Data(data), m_Tick(tick), m_Type(type)
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
			this->m_Data = std::move(other.m_Data);
			return *this;
		}
	}

	bool operator==(const Message& other) const noexcept {

		if (m_Type != other.m_Type)
			return false;
		else if (m_Tick != other.m_Tick)
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
	const char* GetData() const noexcept { return m_Data; }

private:
	MessageType m_Type = MessageType::DEBUG;
	uint64 m_Tick = 0;
	const char* m_Data;
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
	void LogDebug(const char* message) noexcept;
	void LogWarning(const char* message) noexcept;
	void LogError(const char* message) noexcept;

public:
	inline Message* GetNextMessage() noexcept { 

		if (m_LoggedMessages.size() == 0)
			return nullptr;

		Message* message = &m_LoggedMessages.at(m_PrintCursor);
		m_PrintCursor++;
		if (m_PrintCursor == m_LoggedMessages.size()) {
			m_PrintCursor = 0;
			m_IsBufferLooped = true;
		}
		return message;
	}
	inline bool IsBufferLooped() noexcept { return m_IsBufferLooped; }
	inline void NewFrame() noexcept { m_IsBufferLooped = false; }
	inline void Clear() noexcept {

		m_DebugCounter = 0;
		m_WarningCounter = 0;
		m_ErrorCounter = 0;
		m_LoggedMessages.clear();
	}

public:
	inline uint32 GetAllMessagesCount() const noexcept { return m_DebugCounter + m_WarningCounter + m_ErrorCounter; }
	inline uint32 GetDebugMessagesCount() const noexcept { return m_DebugCounter; }
	inline uint32 GetWarningMessagesCount() const noexcept { return m_WarningCounter; }
	inline uint32 GetErrorMessagesCount() const noexcept { return m_ErrorCounter; }

public:
	inline bool GetShowDebugMessages() const noexcept { return m_ShowDebugMessages; }
	inline bool GetShowWarningMessages() const noexcept { return m_ShowWarningMessages; }
	inline bool GetShowErrorMessages() const noexcept { return m_ShowErrorMessages; }

	inline bool& GetShowDebugMessagesRef() noexcept { return m_ShowDebugMessages; }
	inline bool& GetShowWarningMessagesRef() noexcept { return m_ShowWarningMessages; }
	inline bool& GetShowErrorMessagesRef() noexcept { return m_ShowErrorMessages; }

	inline void SetTimeReference(Time& time) noexcept { m_TimeReference = &time; }

private:
	bool m_ShowDebugMessages   = true;
	bool m_ShowWarningMessages = true;
	bool m_ShowErrorMessages   = true;

	bool m_IsBufferLooped      = false;

private:
	uint32 m_DebugLimit     = 100;
	uint32 m_WarningLimit   = 100;
	uint32 m_ErrorLimit     = 100;

	uint32 m_DebugCounter   = 0;
	uint32 m_WarningCounter = 0;
	uint32 m_ErrorCounter   = 0;

	uint32 m_PrintCursor    = 0;

private:
	Time* m_TimeReference = nullptr;
	std::vector<Message> m_LoggedMessages;
};