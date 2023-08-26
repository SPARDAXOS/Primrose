#pragma once
#include "ImGUI/imgui.h"
#include "Utility.hpp"

class Core;
class Editor;
class Logger;
class TextureStorage;
class Time;
class SystemDebuggerWindow;

class Texture2D;

class DebuggerWindow final {
public:
	DebuggerWindow() = delete;
	DebuggerWindow(Core& core, Editor& editor) noexcept;
	~DebuggerWindow() = default;

	DebuggerWindow(const DebuggerWindow&) = delete;
	DebuggerWindow& operator=(const DebuggerWindow&) = delete;

	DebuggerWindow(DebuggerWindow&&) = delete;
	DebuggerWindow& operator=(DebuggerWindow&&) = delete;

public:
	void Render();
	void Init();

public:
	bool GetState() const noexcept { return m_Open; }

private:
	bool m_Open = false;
	bool m_DebugLogWindowReset = true;

private:
	Texture2D* m_DebugTexture		{ nullptr };
	Texture2D* m_WarningTexture		{ nullptr };
	Texture2D* m_ErrorTexture		{ nullptr };

private:
	Core* m_Core									{ nullptr };
	Editor* m_Editor								{ nullptr };
	Logger* m_Logger								{ nullptr };
	TextureStorage* m_TextureStorage				{ nullptr };
	Time* m_Time									{ nullptr };
	ContentBrowser* m_ContentBrowser				{ nullptr };
	SystemDebuggerWindow* m_SystemDebuggerWindow	{ nullptr };
};