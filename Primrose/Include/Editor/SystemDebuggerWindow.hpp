#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class Logger;
class Time;

class SystemDebuggerWindow final {
public:
	SystemDebuggerWindow() = delete;
	SystemDebuggerWindow(Core& core, Editor& editor) noexcept;
	~SystemDebuggerWindow() = default;

	SystemDebuggerWindow(const SystemDebuggerWindow&) = delete;
	SystemDebuggerWindow& operator=(const SystemDebuggerWindow&) = delete;

	SystemDebuggerWindow(SystemDebuggerWindow&&) = delete;
	SystemDebuggerWindow& operator=(SystemDebuggerWindow&&) = delete;

public:
	void Render();
	void Init();

public:
	bool GetState() const noexcept { return m_Open; }

private:
	bool m_Open = false;
	bool m_SystemLogWindowReset = true;

private:
	Core* m_Core						{ nullptr };
	Editor* m_Editor					{ nullptr };
	Logger* m_Logger					{ nullptr };
	Time* m_Time						{ nullptr };
	ContentBrowser* m_ContentBrowser	{ nullptr };
	DebuggerWindow* m_DebuggerWindow	{ nullptr };
};

