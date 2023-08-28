#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class Logger;
class TextureStorage;
class Time;

class ContentBrowser;
class SystemLogWindow;

class Texture2D;

class DebugLogWindow final {
public:
	DebugLogWindow() = delete;
	DebugLogWindow(Core& core, Editor& editor) noexcept;
	~DebugLogWindow() = default;

	DebugLogWindow(const DebugLogWindow&) = delete;
	DebugLogWindow& operator=(const DebugLogWindow&) = delete;

	DebugLogWindow(DebugLogWindow&&) = delete;
	DebugLogWindow& operator=(DebugLogWindow&&) = delete;

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
	Texture2D* m_DebugTexture		{ nullptr };
	Texture2D* m_WarningTexture		{ nullptr };
	Texture2D* m_ErrorTexture		{ nullptr };

private:
	Core* m_Core						{ nullptr };
	Editor* m_Editor					{ nullptr };
	Logger* m_Logger					{ nullptr };
	TextureStorage* m_TextureStorage	{ nullptr };
	Time* m_Time						{ nullptr };
	ContentBrowser* m_ContentBrowser	{ nullptr };
	SystemLogWindow* m_SystemLogWindow	{ nullptr };
};