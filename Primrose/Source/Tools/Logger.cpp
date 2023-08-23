#include "Tools/Logger.hpp"
#include "Systems/Time.hpp"


Logger::Logger(Time& time) noexcept
	: m_Time(&time)
{
}


//TODO: Implement circular c style buffers with fixed sizes for improved performance

void Logger::DebugLog(std::string message) noexcept {

	if (m_Time == nullptr)
		return;

	if (m_DebugCounter == m_DebugLimit)
		m_DebugLog.erase(std::begin(m_DebugLog));
	else
		m_DebugCounter++;
	m_DebugLog.push_back({ MessageType::DEBUG, m_Time->GetTick(), m_Time->GetRunningTime(), message });
}
void Logger::WarningLog(std::string message) noexcept {

	if (m_Time == nullptr)
		return;

	if (m_WarningCounter == m_WarningLimit)
		m_DebugLog.erase(std::begin(m_DebugLog));
	else
		m_WarningCounter++;

	m_DebugLog.push_back({ MessageType::WARNING, m_Time->GetTick(), m_Time->GetRunningTime(), message });
}
void Logger::ErrorLog(std::string message) noexcept {

	if (m_Time == nullptr)
		return;

	if (m_ErrorCounter == m_ErrorLimit)
		m_DebugLog.erase(std::begin(m_DebugLog));
	else
		m_ErrorCounter++;

	m_DebugLog.push_back({ MessageType::ERROR, m_Time->GetTick(), m_Time->GetRunningTime(), message });
}
void Logger::SystemLog(std::string message) noexcept {

	if (m_Time == nullptr)
		return;

	if (m_SystemCounter == m_SystemLimit)
		m_SystemLog.erase(std::begin(m_SystemLog));
	else
		m_SystemCounter++;

	m_SystemLog.push_back({ MessageType::SYSTEM, m_Time->GetTick(), m_Time->GetRunningTime(), message});
}
