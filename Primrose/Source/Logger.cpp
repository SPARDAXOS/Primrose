#include "Logger.hpp"
#include "Time.hpp"


Logger::Logger(Time& time) noexcept
	: m_TimeReference(&time)
{
}


//TODO: Implement circular c style buffers with fixed sizes for improved performance

void Logger::LogDebug(std::string message) noexcept {

	if (m_TimeReference == nullptr)
		return;

	if (m_DebugCounter == m_DebugLimit)
		m_DebugLog.erase(std::begin(m_DebugLog));
	else
		m_DebugCounter++;
	m_DebugLog.push_back({ MessageType::DEBUG, m_TimeReference->GetTick(), m_TimeReference->GetRunningTime(), message });
}
void Logger::LogWarning(std::string message) noexcept {

	if (m_TimeReference == nullptr)
		return;

	if (m_WarningCounter == m_WarningLimit)
		m_DebugLog.erase(std::begin(m_DebugLog));
	else
		m_WarningCounter++;

	m_DebugLog.push_back({ MessageType::WARNING, m_TimeReference->GetTick(), m_TimeReference->GetRunningTime(), message });
}
void Logger::LogError(std::string message) noexcept {

	if (m_TimeReference == nullptr)
		return;

	if (m_ErrorCounter == m_ErrorLimit)
		m_DebugLog.erase(std::begin(m_DebugLog));
	else
		m_ErrorCounter++;

	m_DebugLog.push_back({ MessageType::ERROR, m_TimeReference->GetTick(), m_TimeReference->GetRunningTime(), message });
}
void Logger::LogSystem(std::string message) noexcept {

	if (m_TimeReference == nullptr)
		return;

	if (m_SystemCounter == m_SystemLimit)
		m_SystemLog.erase(std::begin(m_SystemLog));
	else
		m_SystemCounter++;

	m_SystemLog.push_back({ MessageType::SYSTEM, m_TimeReference->GetTick(), m_TimeReference->GetRunningTime(), message});
}
