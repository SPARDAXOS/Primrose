#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class Logger;
class Time;
class ContentBrowser;
class DebugLogWindow;

class SystemLogWindow final {
public:
	SystemLogWindow() = delete;
	SystemLogWindow(Core& core, Editor& editor) noexcept;
	~SystemLogWindow() = default;

	SystemLogWindow(const SystemLogWindow&) = delete;
	SystemLogWindow& operator=(const SystemLogWindow&) = delete;

	SystemLogWindow(SystemLogWindow&&) = delete;
	SystemLogWindow& operator=(SystemLogWindow&&) = delete;

public:
	void Render();
	void Init();

public:
	inline bool GetState() const noexcept { return m_Open; }
	inline bool& GetStateRef() noexcept { return m_Open; }

	inline void ResetWindow() noexcept { m_WindowReset = true; }

private:
	bool m_Open = false;
	bool m_WindowReset = true;

private:
	Core* m_Core						{ nullptr };
	Editor* m_Editor					{ nullptr };
	Logger* m_Logger					{ nullptr };
	Time* m_Time						{ nullptr };
	ContentBrowser* m_ContentBrowser	{ nullptr };
	DebugLogWindow* m_DebugLogWindow	{ nullptr };
};

