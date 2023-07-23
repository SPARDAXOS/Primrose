#include "Logger.hpp"
#include "Time.hpp"


Logger::Logger(Time& time) noexcept
	: m_TimeReference(&time)
{
}


void Logger::LogDebug(const char* message) noexcept {

	if (m_TimeReference == nullptr)
		return;

	if (m_DebugCounter == m_DebugLimit)
		m_LoggedMessages.erase(std::begin(m_LoggedMessages));

	m_DebugCounter++;
	m_LoggedMessages.push_back({ MessageType::DEBUG, m_TimeReference->GetTick(), message });
}
void Logger::LogWarning(const char* message) noexcept {

	if (m_TimeReference == nullptr)
		return;

	if (m_WarningCounter == m_WarningLimit)
		m_LoggedMessages.erase(std::begin(m_LoggedMessages));

	m_WarningCounter++;
	m_LoggedMessages.push_back({ MessageType::WARNING, m_TimeReference->GetTick(), message });
}
void Logger::LogError(const char* message) noexcept {

	if (m_TimeReference == nullptr)
		return;

	if (m_ErrorCounter == m_ErrorLimit)
		m_LoggedMessages.erase(std::begin(m_LoggedMessages));

	m_ErrorCounter++;
	m_LoggedMessages.push_back({ MessageType::ERROR, m_TimeReference->GetTick(), message });
}

